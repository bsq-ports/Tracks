pub extern crate tracks_rs;

use android_logger::Config;
use log::{error, info, LevelFilter};
use std::backtrace::Backtrace;
use std::ffi::CString;
use std::panic::PanicHookInfo;

// Static variable to hold the panic callback function
static mut PANIC_CALLBACK: Option<extern "C" fn(*const std::os::raw::c_char)> = None;

/// Set the C function that will be called when a panic occurs
#[unsafe(no_mangle)]
pub unsafe extern "C" fn set_panic_callback(callback: extern "C" fn(*const std::os::raw::c_char)) {
    unsafe {
        PANIC_CALLBACK = Some(callback);
    }
}

#[ctor::ctor]
fn main() {
    android_logger::init_once(Config::default().with_max_level(LevelFilter::Trace));

    std::panic::set_hook(panic_hook(true, true));
}

/// Returns a panic handler, optionally with backtrace and spantrace capture.
pub fn panic_hook(
    backtrace: bool,
    spantrace: bool,
) -> Box<dyn Fn(&PanicHookInfo) + Send + Sync + 'static> {
    // Mostly taken from https://doc.rust-lang.org/src/std/panicking.rs.html
    Box::new(move |info| {
        let location = info.location().unwrap();
        let msg = match info.payload().downcast_ref::<&'static str>() {
            Some(s) => *s,
            None => match info.payload().downcast_ref::<String>() {
                Some(s) => &s[..],
                None => "Box<dyn Any>",
            },
        };

        if let Some(callback) = unsafe { PANIC_CALLBACK } {
            let message = format!(
                "panicked at '{}', {}: {}:{}",
                msg,
                location.file(),
                location.line(),
                location.column()
            );
            let backtrace = if backtrace {
                format!("\nBacktrace:\n{:#?}", Backtrace::force_capture())
            } else {
                String::new()
            };

            let finished = CString::new(format!("{}\n{}", message, backtrace)).unwrap();

            callback(finished.as_ptr());
        }

        info!(target: "panic", "panicked at '{}', {}", msg, location);
        if backtrace {
            error!(target: "panic", "{:?}", Backtrace::force_capture());
        }
        if spantrace {
            // error!(target: "panic", "{:?}", SpanTrace::capture());
        }
    })
}
