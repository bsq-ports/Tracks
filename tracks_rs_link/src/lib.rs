pub extern crate tracks_rs;

use android_logger::Config;
use log::{LevelFilter, error, info};
use std::backtrace::Backtrace;
use std::ffi::CString;
use std::panic::PanicHookInfo;

// Static variable to hold the panic callback function
static mut PANIC_CALLBACK: Option<extern "C" fn(*const std::os::raw::c_char)> = None;

/// Set the C function that will be called when a panic occurs
///
/// # Safety
/// - `callback` must be a valid function pointer with the C ABI and remain valid for as long as it may be invoked.
/// - The callback will be invoked from Rust's panic hook; it should be safe to call from the thread/context the panic hook executes on.
/// - The caller is responsible for ensuring `callback` is safe to call concurrently if the panic hook can run on multiple threads.
#[unsafe(no_mangle)]
pub unsafe extern "C" fn set_panic_callback(callback: extern "C" fn(*const std::os::raw::c_char)) {
    unsafe {
        PANIC_CALLBACK = Some(callback);
    }
}

#[ctor::ctor]
fn main() {
    if paper2_tracing::init_paper_tracing().is_err() {
        // fallback to android_logger if tracing initialization fails
        android_logger::init_once(Config::default().with_max_level(LevelFilter::Trace));
        info!("Failed to initialize paper2 tracing, falling back to android_logger");
    };

    info!("tracks_rs_link initialized");
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

            let finished = CString::new(format!("{message}\n{backtrace}")).unwrap();

            callback(finished.as_ptr());
        }

        info!(target: "panic", "panicked at '{msg}', {location}");
        if backtrace {
            error!(target: "panic", "{:?}", Backtrace::force_capture());
        }
        if spantrace {
            // error!(target: "panic", "{:?}", SpanTrace::capture());
        }
    })
}
