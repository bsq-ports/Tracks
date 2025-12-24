#pragma once

#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#ifdef __cplusplus
namespace Tracks {
namespace ffi {
#endif  // __cplusplus

typedef enum WrapBaseValueType {
  Unknown = -1,
  Vec3 = 0,
  Quat = 1,
  Vec4 = 2,
  Float = 3,
} WrapBaseValueType;

typedef enum Functions {
  EaseLinear,
  EaseStep,
  EaseInQuad,
  EaseOutQuad,
  EaseInOutQuad,
  EaseInCubic,
  EaseOutCubic,
  EaseInOutCubic,
  EaseInQuart,
  EaseOutQuart,
  EaseInOutQuart,
  EaseInQuint,
  EaseOutQuint,
  EaseInOutQuint,
  EaseInSine,
  EaseOutSine,
  EaseInOutSine,
  EaseInCirc,
  EaseOutCirc,
  EaseInOutCirc,
  EaseInExpo,
  EaseOutExpo,
  EaseInOutExpo,
  EaseInElastic,
  EaseOutElastic,
  EaseInOutElastic,
  EaseInBack,
  EaseOutBack,
  EaseInOutBack,
  EaseInBounce,
  EaseOutBounce,
  EaseInOutBounce,
} Functions;

enum CEventTypeEnum
#ifdef __cplusplus
  : uint32_t
#endif // __cplusplus
 {
  AnimateTrack = 0,
  AssignPathAnimation = 1,
};
#ifndef __cplusplus
typedef uint32_t CEventTypeEnum;
#endif // __cplusplus

/**
 * An enumeration of common property names used in Tracks.
 */
enum PropertyNames
#ifdef __cplusplus
  : uint32_t
#endif // __cplusplus
 {
  Position,
  OffsetPosition,
  Rotation,
  Scale,
  LocalRotation,
  LocalPosition,
  DefinitePosition,
  Dissolve,
  DissolveArrow,
  Time,
  Cuttable,
  Color,
  Attentuation,
  FogOffset,
  HeightFogStartY,
  HeightFogHeight,
  UnknownPropertyName,
};
#ifndef __cplusplus
typedef uint32_t PropertyNames;
#endif // __cplusplus

typedef enum CEventPropertyIdType {
  CString = 0,
  PropertyName = 1,
} CEventPropertyIdType;

/**
 * JSON FFI
 */
typedef enum JsonValueType {
  Number,
  Null,
  String,
  Array,
} JsonValueType;

typedef struct BaseFFIProviderValues BaseFFIProviderValues;

/**
 * Point definitions are used to describe what happens over the course of an animation,
 * they are used slightly differently for different properties.
 * They consist of a collection of points over time.
 */
typedef struct BasePointDefinition BasePointDefinition;

/**
 * Context for base value providers
 * Holds all the base values that can be accessed
 * by base value providers
 *
 * This context is passed to the value providers
 * to get the current base values
 */
typedef struct BaseProviderContext BaseProviderContext;

typedef struct CoroutineManager CoroutineManager;

typedef struct EventData EventData;

typedef struct FloatPointDefinition FloatPointDefinition;

/**
 * A structure to manage interpolation between two point definitions over time.
 */
typedef struct PointDefinitionInterpolation PointDefinitionInterpolation;

typedef struct QuaternionPointDefinition QuaternionPointDefinition;

/**
 * A Track represents a collection of properties and path properties associated with game objects.
 * It allows registering, retrieving, and managing properties and game objects.
 */
typedef struct Track Track;

typedef struct TracksHolder TracksHolder;

typedef struct ValueProperty ValueProperty;

typedef struct Vector3PointDefinition Vector3PointDefinition;

typedef struct Vector4PointDefinition Vector4PointDefinition;

typedef struct WrappedValues {
  const float *values;
  uintptr_t length;
} WrappedValues;

typedef struct WrappedValues (*BaseFFIProvider)(const struct BaseProviderContext*, void*);

typedef struct WrapVec3 {
  float x;
  float y;
  float z;
} WrapVec3;

typedef struct WrapQuat {
  float x;
  float y;
  float z;
  float w;
} WrapQuat;

typedef struct WrapVec4 {
  float x;
  float y;
  float z;
  float w;
} WrapVec4;

typedef union WrapBaseValueUnion {
  struct WrapVec3 vec3;
  struct WrapQuat quat;
  struct WrapVec4 vec4;
  float float_v;
} WrapBaseValueUnion;

typedef struct WrapBaseValue {
  enum WrapBaseValueType ty;
  union WrapBaseValueUnion value;
} WrapBaseValue;

typedef union CEventPropertyId {
  const char *property_str;
  PropertyNames property_name;
} CEventPropertyId;

typedef struct CEventType {
  CEventTypeEnum ty;
  union CEventPropertyId property_id;
  enum CEventPropertyIdType property_id_type;
} CEventType;

typedef struct TrackKeyFFI {
  uint64_t _0;
} TrackKeyFFI;

typedef struct CEventData {
  float raw_duration;
  enum Functions easing;
  uint32_t repeat;
  float start_time;
  struct CEventType event_type;
  struct TrackKeyFFI track_key;
  /**
   * nullable pointer to BasePointDefinition
   */
  const struct BasePointDefinition *point_data_ptr;
} CEventData;

typedef struct JsonArray {
  const struct FFIJsonValue *elements;
  uintptr_t length;
} JsonArray;

typedef union JsonValueData {
  double number_value;
  const char *string_value;
  const struct JsonArray *array;
} JsonValueData;

typedef struct FFIJsonValue {
  enum JsonValueType value_type;
  union JsonValueData data;
} FFIJsonValue;

typedef struct FloatInterpolationResult {
  float value;
  bool is_last;
} FloatInterpolationResult;

typedef struct QuaternionInterpolationResult {
  struct WrapQuat value;
  bool is_last;
} QuaternionInterpolationResult;

typedef struct Vector3InterpolationResult {
  struct WrapVec3 value;
  bool is_last;
} Vector3InterpolationResult;

typedef struct Vector4InterpolationResult {
  struct WrapVec4 value;
  bool is_last;
} Vector4InterpolationResult;

typedef struct PointDefinitionInterpolation PathProperty;

typedef struct CValueNullable {
  bool has_value;
  struct WrapBaseValue value;
} CValueNullable;

typedef struct CTimeUnit {
  uint64_t _0;
  uint32_t _1;
} CTimeUnit;

typedef struct CValueProperty {
  struct CValueNullable value;
  struct CTimeUnit last_updated;
} CValueProperty;

typedef struct GameObject {
  const void *ptr;
} GameObject;

typedef struct CPropertiesMap {
  struct ValueProperty *position;
  struct ValueProperty *rotation;
  struct ValueProperty *scale;
  struct ValueProperty *local_rotation;
  struct ValueProperty *local_position;
  struct ValueProperty *dissolve;
  struct ValueProperty *dissolve_arrow;
  struct ValueProperty *time;
  struct ValueProperty *cuttable;
  struct ValueProperty *color;
  struct ValueProperty *attentuation;
  struct ValueProperty *fog_offset;
  struct ValueProperty *height_fog_start_y;
  struct ValueProperty *height_fog_height;
} CPropertiesMap;

typedef struct CPathPropertiesMap {
  PathProperty *position;
  PathProperty *rotation;
  PathProperty *scale;
  PathProperty *local_rotation;
  PathProperty *local_position;
  PathProperty *definite_position;
  PathProperty *dissolve;
  PathProperty *dissolve_arrow;
  PathProperty *cuttable;
  PathProperty *color;
} CPathPropertiesMap;

typedef struct Vec3Option {
  struct WrapVec3 value;
  bool has_value;
} Vec3Option;

typedef struct QuatOption {
  struct WrapQuat value;
  bool has_value;
} QuatOption;

typedef struct FloatOption {
  float value;
  bool has_value;
} FloatOption;

typedef struct Vec4Option {
  struct WrapVec4 value;
  bool has_value;
} Vec4Option;

typedef struct CPropertiesValues {
  struct Vec3Option position;
  struct QuatOption rotation;
  struct Vec3Option scale;
  struct QuatOption local_rotation;
  struct Vec3Option local_position;
  struct FloatOption dissolve;
  struct FloatOption dissolve_arrow;
  struct FloatOption time;
  struct FloatOption cuttable;
  struct Vec4Option color;
  struct FloatOption attentuation;
  struct FloatOption fog_offset;
  struct FloatOption height_fog_start_y;
  struct FloatOption height_fog_height;
} CPropertiesValues;

typedef struct CPathPropertiesValues {
  struct Vec3Option position;
  struct QuatOption rotation;
  struct Vec3Option scale;
  struct QuatOption local_rotation;
  struct Vec3Option local_position;
  struct FloatOption definite_position;
  struct FloatOption dissolve;
  struct FloatOption dissolve_arrow;
  struct FloatOption cuttable;
  struct Vec4Option color;
} CPathPropertiesValues;

typedef void (*CGameObjectCallback)(struct GameObject, bool, void*);



#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/**
 * Create a new `BaseProviderContext` and return a raw pointer to it.
 */
struct BaseProviderContext *base_provider_context_create(void);

/**
 * Destroy a `BaseProviderContext` previously returned by `base_provider_context_create`.
 */
void base_provider_context_destroy(struct BaseProviderContext *ctx);

/**
 * Create a `BaseFFIProviderValues` wrapper from a C function pointer and user value.
 *
 * # Safety
 * - `func` must be a valid pointer to a `BaseFFIProvider` function table and not null.
 * - `user_value` is passed through as-is and its ownership remains with the caller.
 */
struct BaseFFIProviderValues *tracks_make_base_ffi_provider(const BaseFFIProvider *func,
                                                            void *user_value);

/**
 * Set a base provider value by name. `value` is a `WrapBaseValue` (C layout) converted into `BaseValue`.
 */
void base_provider_context_set_value(struct BaseProviderContext *ctx,
                                     const char *base,
                                     struct WrapBaseValue value);

/**
 * Get a base provider value by name as a `WrapBaseValue`.
 * The returned `WrapBaseValue` points into data owned by `ctx` (via slice pointer), callers must not free it.
 */
struct WrapBaseValue base_provider_context_get_value(const struct BaseProviderContext *ctx,
                                                     const char *base);

/**
 * Get base provider values as a pointer+length pair. The returned `WrappedValues` borrows data from `ctx`.
 */
struct WrappedValues base_provider_context_get_values_array(const struct BaseProviderContext *ctx,
                                                            const char *base);

/**
 * Get the type of the base provider value for `base` (Vec3/Quat/Vec4/Float)
 */
enum WrapBaseValueType base_provider_context_get_type(const struct BaseProviderContext *ctx,
                                                      const char *base);

/**
 * Creates a new CoroutineManager instance and returns a raw pointer to it.
 * The caller is responsible for freeing the memory using destroy_coroutine_manager.
 */
struct CoroutineManager *create_coroutine_manager(void);

/**
 * Destroys a `CoroutineManager` instance, freeing its memory.
 *
 * # Safety
 * - `manager` must be a pointer previously returned by `create_coroutine_manager` and not already freed.
 * - Passing a null pointer is a no-op.
 */
void destroy_coroutine_manager(struct CoroutineManager *manager);

/**
 * Starts an event coroutine in the manager. Consumes `event_data`.
 *
 * # Safety
 * - `manager` must be a valid pointer to a `CoroutineManager`.
 * - `context` must be a valid pointer to a `BaseProviderContext` for the duration of the call.
 * - `tracks_holder` must be a valid pointer to a `TracksHolder`.
 * - `event_data` must be a pointer returned by `event_data_to_rust`. The data is cloned, so the caller retains ownership.
 */
void start_event_coroutine(struct CoroutineManager *manager,
                           float bpm,
                           float song_time,
                           const struct BaseProviderContext *context,
                           struct TracksHolder *tracks_holder,
                           const struct EventData *event_data);

/**
 * Polls all events in the manager, updating their state based on the current song time.
 *
 * # Safety
 * - `manager` must be a valid pointer to a `CoroutineManager`.
 * - `context` must be a valid pointer to a `BaseProviderContext`.
 * - `tracks_holder` must be a valid pointer to a `TracksHolder`.
 */
void poll_events(struct CoroutineManager *manager,
                 float song_time,
                 const struct BaseProviderContext *context,
                 struct TracksHolder *tracks_holder);

/**
 * C-compatible wrapper for easing functions
 */
float interpolate_easing(enum Functions easing_function, float t);

/**
 * Gets an easing function by index (useful for FFI where enums might be troublesome)
 * Returns Functions::EaseLinear if the index is out of bounds
 */
enum Functions get_easing_function_by_index(int32_t index);

/**
 * Gets the total number of available easing functions
 */
int32_t get_easing_function_count(void);

/**
 * Converts a `CEventData` into a Rust `EventData`.
 * Does not consume the input struct; returns an owned pointer to a newly allocated `EventData`.
 *
 * # Safety
 * - `c_event_data` must be a valid, non-null pointer to a `CEventData`.
 * - Any C strings referenced inside `c_event_data` must be valid null-terminated pointers.
 * - The returned pointer is owned by the caller and must be freed by calling `event_data_dispose`.
 */
struct EventData *event_data_to_rust(const struct CEventData *c_event_data);

/**
 * Dispose of an `EventData` previously returned by `event_data_to_rust`.
 *
 * # Safety
 * - `event_data` must be a pointer previously returned by `event_data_to_rust` and not already freed.
 * - Passing a null pointer is a no-op.
 */
void event_data_dispose(struct EventData *event_data);

/**
 * Create a JSON number value.
 *
 * # Safety
 * - This function is FFI-safe and returns an owned `FFIJsonValue` by value.
 */
struct FFIJsonValue tracks_create_json_number(double value);

/**
 * Create a JSON string value referencing a C string pointer.
 *
 * # Safety
 * - `value` must be a valid null-terminated C string pointer for the duration of use on the consumer side.
 * - The returned `FFIJsonValue` stores the pointer as-is; ownership of the string remains with the caller.
 */
struct FFIJsonValue tracks_create_json_string(const char *value);

/**
 * Create a JSON array value by allocating a `JsonArray` that wraps the elements pointer.
 *
 * # Safety
 * - `elements` must point to an array of `FFIJsonValue` of length `length` and remain valid until the caller frees the `FFIJsonValue`.
 * - The caller is responsible for calling `tracks_free_json_value` to free the leaked `JsonArray`.
 */
struct FFIJsonValue tracks_create_json_array(const struct FFIJsonValue *elements,
                                             uintptr_t length);

/**
 * Free an `FFIJsonValue` previously produced that may contain heap allocations.
 *
 * # Safety
 * - `json_value` must point to an `FFIJsonValue` produced by the corresponding create functions and not already freed.
 * - This implementation only frees the top-level `JsonArray` if present; nested structures are not recursively freed.
 */
void tracks_free_json_value(struct FFIJsonValue *json_value);

/**
 * BASE POINT DEFINITION
 *
 * # Safety
 * - `json` must be a valid pointer to an `FFIJsonValue` or null if not used by the specific constructor.
 * - `context` must be a valid, non-null pointer to a live `BaseProviderContext` for the duration of this call.
 * - The returned pointer is owned by the caller and must be freed by calling `base_point_definition_free`.
 * - This function may panic on invalid input; unwinding across the FFI boundary is undefined behaviour.
 */
struct BasePointDefinition *tracks_make_base_point_definition(const struct FFIJsonValue *json,
                                                              enum WrapBaseValueType ty,
                                                              struct BaseProviderContext *context);

/**
 * BASE POINT DEFINITION FREE
 *
 * # Safety
 * - `point_definition` must be a pointer previously returned by `tracks_make_base_point_definition`.
 * - After calling this function the pointer is invalid and must not be used again.
 * - Passing a null pointer is allowed and is a no-op.
 */
void base_point_definition_free(struct BasePointDefinition *point_definition);

/**
 * Interpolate a base point definition at a given time.
 *
 * # Safety
 * - `point_definition` must be a valid, non-null pointer to a `BasePointDefinition`.
 * - `is_last_out` must be a valid, non-null pointer to a `bool` to receive the `is_last` flag.
 * - `context` must be a valid pointer to `BaseProviderContext` for the duration of the call.
 * - This function does not take ownership of any pointers passed in.
 * - Do not rely on the returned `WrapBaseValue` containing any borrowed references; it is an owned value.
 */
struct WrapBaseValue tracks_interpolate_base_point_definition(const struct BasePointDefinition *point_definition,
                                                              float time,
                                                              bool *is_last_out,
                                                              struct BaseProviderContext *context);

/**
 * Return number of points in the point definition.
 *
 * Safety:
 * - `point_definition` must be a valid, non-null pointer to a `BasePointDefinition`.
 */
uintptr_t tracks_base_point_definition_count(const struct BasePointDefinition *point_definition);

/**
 * Check whether the point definition references a base provider.
 *
 * Safety:
 * - `point_definition` must be a valid, non-null pointer to a `BasePointDefinition`.
 */
bool tracks_base_point_definition_has_base_provider(const struct BasePointDefinition *point_definition);

/**
 * Get the `WrapBaseValueType` of the point definition.
 * Safety:
 * - `point_definition` must be a valid, non-null pointer to a `BasePointDefinition`.
 */
enum WrapBaseValueType tracks_base_point_definition_get_type(const struct BasePointDefinition *point_definition);

/**
 * FLOAT POINT DEFINITION
 *
 * # Safety
 * - `json` may be null; if non-null it must point to a valid `FFIJsonValue`.
 * - `context` must be a valid pointer to a `BaseProviderContext`.
 */
const struct FloatPointDefinition *tracks_make_float_point_definition(const struct FFIJsonValue *json,
                                                                      struct BaseProviderContext *context);

/**
 * Interpolate a float point definition at `time`.
 *
 * # Safety
 * - `point_definition` must be a valid pointer to a `FloatPointDefinition`.
 * - `context` must be a valid pointer to a `BaseProviderContext`.
 */
struct FloatInterpolationResult tracks_interpolate_float(const struct FloatPointDefinition *point_definition,
                                                         float time,
                                                         struct BaseProviderContext *context);

/**
 * # Safety
 * - `point_definition` must be a valid pointer to a `FloatPointDefinition`.
 */
uintptr_t tracks_float_count(const struct FloatPointDefinition *point_definition);

/**
 * # Safety
 * - `point_definition` must be a valid pointer to a `FloatPointDefinition`.
 */
bool tracks_float_has_base_provider(const struct FloatPointDefinition *point_definition);

/**
 * QUATERNION POINT DEFINITION
 *
 * # Safety
 * - `json` may be null; if non-null it must point to a valid `FFIJsonValue`.
 * - `context` must be a valid pointer to a `BaseProviderContext`.
 */
const struct QuaternionPointDefinition *tracks_make_quat_point_definition(const struct FFIJsonValue *json,
                                                                          struct BaseProviderContext *context);

/**
 * Interpolate a Quaternion point definition at `time`.
 *
 * # Safety
 * - `point_definition` must be a valid pointer to a `QuaternionPointDefinition`.
 * - `context` must be a valid pointer to a `BaseProviderContext`.
 */
struct QuaternionInterpolationResult tracks_interpolate_quat(const struct QuaternionPointDefinition *point_definition,
                                                             float time,
                                                             struct BaseProviderContext *context);

/**
 * # Safety
 * - `point_definition` must be a valid pointer to a `QuaternionPointDefinition`.
 */
uintptr_t tracks_quat_count(const struct QuaternionPointDefinition *point_definition);

/**
 * # Safety
 * - `point_definition` must be a valid pointer to a `QuaternionPointDefinition`.
 */
bool tracks_quat_has_base_provider(const struct QuaternionPointDefinition *point_definition);

/**
 * VECTOR3 POINT DEFINITION
 *
 * # Safety
 * - `json` may be null; if non-null it must point to a valid `FFIJsonValue`.
 * - `context` must be a valid pointer to a `BaseProviderContext`.
 */
const struct Vector3PointDefinition *tracks_make_vector3_point_definition(const struct FFIJsonValue *json,
                                                                          struct BaseProviderContext *context);

/**
 * Interpolate a Vector3 point definition at `time`.
 *
 * # Safety
 * - `point_definition` must be a valid pointer to a `Vector3PointDefinition`.
 * - `context` must be a valid pointer to a `BaseProviderContext`.
 */
struct Vector3InterpolationResult tracks_interpolate_vector3(const struct Vector3PointDefinition *point_definition,
                                                             float time,
                                                             struct BaseProviderContext *context);

/**
 * # Safety
 * - `point_definition` must be a valid pointer to a `Vector3PointDefinition`.
 */
uintptr_t tracks_vector3_count(const struct Vector3PointDefinition *point_definition);

/**
 * # Safety
 * - `point_definition` must be a valid pointer to a `Vector3PointDefinition`.
 */
bool tracks_vector3_has_base_provider(const struct Vector3PointDefinition *point_definition);

/**
 * VECTOR4 POINT DEFINITION
 *
 * # Safety
 * - `json` may be null; if non-null it must point to a valid `FFIJsonValue`.
 * - `context` must be a valid pointer to a `BaseProviderContext`.
 */
const struct Vector4PointDefinition *tracks_make_vector4_point_definition(const struct FFIJsonValue *json,
                                                                          struct BaseProviderContext *context);

/**
 * Interpolate a Vector4 point definition at `time`.
 *
 * # Safety
 * - `point_definition` must be a valid pointer to a `Vector4PointDefinition`.
 * - `context` must be a valid pointer to a `BaseProviderContext`.
 */
struct Vector4InterpolationResult tracks_interpolate_vector4(const struct Vector4PointDefinition *point_definition,
                                                             float time,
                                                             struct BaseProviderContext *context);

/**
 * # Safety
 * - `point_definition` must be a valid pointer to a `Vector4PointDefinition`.
 */
uintptr_t tracks_vector4_count(const struct Vector4PointDefinition *point_definition);

/**
 * # Safety
 * - `point_definition` must be a valid pointer to a `Vector4PointDefinition`.
 */
bool tracks_vector4_has_base_provider(const struct Vector4PointDefinition *point_definition);

PathProperty *path_property_create(void);

void path_property_finish(PathProperty *ptr);

PropertyNames string_to_property_name(const char *ptr);

/**
 * # Safety
 * - `ptr` must be a valid pointer to a `PathProperty` created by `path_property_create`.
 * - After calling this function the `PathProperty` remains owned by the caller; this function only performs finalization.
 */
void path_property_init(PathProperty *ptr,
                        struct BasePointDefinition *new_point_data);

/**
 * # Safety
 * - `ptr` must be a valid pointer to a `PathProperty`.
 * - `new_point_data`, if non-null, must point to a valid `BasePointDefinition` and ownership of its contents may be moved.
 *
 * Consumes the path property and frees its memory.
 */
void path_property_free(PathProperty *ptr);

/**
 * # Safety
 * - `ptr` must be a pointer previously returned by `path_property_create` and not already freed.
 * - Passing null is a no-op.
 */
float path_property_get_time(const PathProperty *ptr);

/**
 * # Safety
 * - `ptr` must be a valid pointer to a `PathProperty`.
 */
void path_property_set_time(PathProperty *ptr, float time);

/**
 * # Safety
 * - `ptr` must be a valid, non-null pointer to a `PathProperty`.
 */
struct CValueNullable path_property_interpolate(PathProperty *ptr,
                                                float time,
                                                const struct BaseProviderContext *context);

/**
 * # Safety
 * - `ptr` must be a valid pointer to a `PathProperty`.
 * - `context` must be a valid pointer to a `BaseProviderContext` for the duration of the call.
 */
enum WrapBaseValueType path_property_get_type(const PathProperty *ptr);

/**
 * # Safety
 * - `ptr` may be null; if non-null it must point to a valid `PathProperty`.
 */
enum WrapBaseValueType property_get_type(const struct ValueProperty *ptr);

/**
 * # Safety
 * - `ptr` may be null; if non-null it must point to a valid `ValueProperty`.
 */
struct CValueProperty property_get_value(const struct ValueProperty *ptr);

/**
 * # Safety
 * - `ptr` may be null; if non-null it must point to a valid `ValueProperty`.
 */
struct CTimeUnit property_get_last_updated(const struct ValueProperty *ptr);

struct CTimeUnit get_time(void);

/**
 * Create a new `Track` and return a raw pointer to it.
 *
 * # Safety
 * - The returned pointer is owned by the caller and must be freed with `track_destroy`.
 * - The caller must not free the pointer by other means or use it after calling `track_destroy`.
 * - This function is FFI-safe but the returned pointer is not thread-safe; use from the same thread unless synchronized.
 */
struct Track *track_create(void);

/**
 * Consumes the track and frees its memory.
 * Destroy a `Track` previously returned by `track_create`.
 *
 * # Safety
 * - `track` must be a pointer previously returned by `track_create` and not already freed.
 * - Passing a null pointer is a no-op.
 */
void track_destroy(struct Track *track);

/**
 * Reset a track to its default state.
 *
 * # Safety
 * - `track` must be a valid, non-null pointer to a `Track`.
 * - This function mutates the pointed-to `Track`.
 */
void track_reset(struct Track *track);

/**
 * Set the name of a track from a C string.
 *
 * # Safety
 * - `track` must be a valid, non-null pointer to a `Track`.
 * - `name` must be a valid null-terminated C string pointer.
 * - The function copies the string contents; the caller retains ownership of `name`.
 */
void track_set_name(struct Track *track, const char *name);

/**
 * Return the name of the track as a newly allocated C string.
 *
 * # Safety
 * - The returned pointer is owned by the caller and MUST be freed using `CString::from_raw` on the caller side when no longer needed.
 * - The function may return null on error or if `track` is null.
 */
const char *track_get_name(const struct Track *track);

/**
 * Register a `GameObject` with the track.
 *
 * # Safety
 * - `track` must be a valid, non-null pointer to a `Track`.
 * - `game_object` is passed by value; ensure it is valid according to the `GameObject` ABI.
 */
void track_register_game_object(struct Track *track, struct GameObject game_object);

/**
 * Unregister a `GameObject` from the track.
 *
 * # Safety
 * - `track` must be a valid, non-null pointer to a `Track`.
 * - `game_object` must be a `GameObject` compatible with the track's internal representation.
 */
void track_unregister_game_object(struct Track *track, struct GameObject game_object);

/**
 * Get a pointer to the track's `GameObject` array and write its length to `size`.
 *
 * # Safety
 * - `track` must be a valid, non-null pointer to a `Track`.
 * - `size` must be a valid, non-null pointer to `usize` and will be written to.
 * - The returned pointer is valid while the `Track` is alive and not mutated; do not hold it across mutations that can reallocate.
 */
const struct GameObject *track_get_game_objects(const struct Track *track,
                                                uintptr_t *size);

/**
 * Register a value property with the given id on the track.
 *
 * # Safety
 * - `track` must be a valid, non-null pointer to a `Track`.
 * - `id` must be a valid null-terminated C string.
 * - `property` must be a valid pointer to a `ValueProperty`; the function clones the value.
 */
void track_register_property(struct Track *track, const char *id, struct ValueProperty *property);

/**
 * Get a mutable pointer to a registered `ValueProperty` by id.
 *
 * # Safety
 * - `track` must be a valid, non-null pointer to a `Track`.
 * - `id` must be a valid null-terminated C string.
 * - The returned pointer is valid while the property remains registered and the `Track` is not mutated in a way that invalidates it.
 */
struct ValueProperty *track_get_property(struct Track *track,
                                         const char *id);

/**
 * Get a mutable pointer to a predefined property by `PropertyNames`.
 *
 * # Safety
 * - `track` must be a valid, non-null pointer to a `Track`.
 * - The returned pointer is valid while the property remains registered; do not hold it across mutating operations.
 */
struct ValueProperty *track_get_property_by_name(struct Track *track,
                                                 PropertyNames id);

/**
 * Get a mutable pointer to a predefined path property by `PropertyNames`.
 *
 * # Safety
 * - `track` must be a valid, non-null pointer to a `Track`.
 */
PathProperty *track_get_path_property_by_name(struct Track *track, PropertyNames id);

/**
 * Register a path property on the track.
 *
 * # Safety
 * - `track` must be a valid pointer to a `Track`.
 * - `id` must be a valid C string.
 * - `property` must be a valid pointer to a `PathProperty`; the property value is moved from the pointer.
 */
void track_register_path_property(struct Track *track,
                                  const char *id,
                                  const PathProperty *property);

/**
 * Get a mutable pointer to a path property by id.
 *
 * # Safety
 * - `track` must be a valid pointer to a `Track`.
 * - `id` must be a valid C string.
 */
PathProperty *track_get_path_property(struct Track *track, const char *id);

/**
 * Return a `CPropertiesMap` with pointers into the track's registered properties.
 *
 * Safety:
 * - `track` must be a valid, non-null pointer to a `Track`.
 * - The returned pointers are valid only while the `Track` is alive and not mutated in a way that moves or removes the properties.
 * - Do not retain these pointers across calls that might mutate the track.
 */
struct CPropertiesMap track_get_properties_map(struct Track *track);

/**
 * Return a `CPathPropertiesMap` with pointers into the track's path properties.
 *
 * Safety:
 * - `track` must be a valid, non-null pointer to a `Track`.
 * - Returned pointers are valid only while the track's path properties remain in-place.
 */
struct CPathPropertiesMap track_get_path_properties_map(struct Track *track);

/**
 * Return a `CPropertiesValues` with the current values of the track's properties.
 * - `track` must be a valid, non-null pointer to a `Track
 * - The returned struct contains copies of the current property values.
 */
struct CPropertiesValues track_get_properties_values(struct Track *track);

/**
 * Return a `CPathPropertiesValues` with the interpolated values of the track's path properties at the given time.
 * Safety:
 * - `track` must be a valid, non-null pointer to a `Track`.
 * - `ctx` must be a valid, non-null pointer to a `BaseProviderContext`.
 *
 * - The returned struct contains copies of the interpolated property values.
 *
 */
struct CPathPropertiesValues track_get_path_properties_values(struct Track *track,
                                                              float time,
                                                              const struct BaseProviderContext *ctx);

/**
 * Register a C callback to be invoked when a game object is added/removed.
 *
 * Safety:
 * - `track` must be a valid pointer to a `Track`.
 * - `callback` and `user_data` must remain valid for as long as the callback may be invoked.
 * - The returned pointer is an opaque handle to the stored Rust closure; it must be removed with `track_remove_game_object_callback`.
 * - The callback is invoked on the Rust side; ensure `callback` is safe to call from Rust execution context.
 *
 * - the callback signature is `extern "C" fn(GameObject, bool, *mut c_void)` where the bool indicates if the object was added (true) or removed (false).
 */
void (**track_register_game_object_callback(struct Track *track,
                                            CGameObjectCallback callback,
                                            void *user_data))(struct GameObject, bool);

/**
 * Remove a previously registered game object callback.
 *
 * Safety:
 * - `track` must be a valid pointer to a `Track`.
 * - `callback` must be a pointer previously returned by `track_register_game_object_callback`.
 * - After calling this function the `callback` pointer must not be used again.
 */
void track_remove_game_object_callback(struct Track *track, void (**callback)(struct GameObject,
                                                                              bool));

/**
 * Create a new `TracksHolder` and return a raw pointer to it.
 */
struct TracksHolder *tracks_holder_create(void);

/**
 * Destroy a `TracksHolder` previously returned by `tracks_holder_create`.
 */
void tracks_holder_destroy(struct TracksHolder *holder);

/**
 * Add a `Track` to the holder. Takes ownership of the `Track` pointer passed in.
 * Returns a `TrackKeyFFI` identifying the inserted track, or null-equivalent on error.
 */
struct TrackKeyFFI tracks_holder_add_track(struct TracksHolder *holder, struct Track *track);

/**
 * Get an immutable pointer to a `Track` by `TrackKeyFFI`.
 */
const struct Track *tracks_holder_get_track(const struct TracksHolder *holder,
                                            struct TrackKeyFFI key);

/**
 * Get a mutable pointer to a `Track` by `TrackKeyFFI`.
 */
struct Track *tracks_holder_get_track_mut(struct TracksHolder *holder, struct TrackKeyFFI key);

/**
 * Look up a track by name and return a pointer to it (const).
 */
const struct Track *tracks_holder_get_track_by_name(const struct TracksHolder *holder,
                                                    const char *name);

/**
 * Get the `TrackKeyFFI` for a track with the given name, or null-equivalent if not found.
 */
struct TrackKeyFFI tracks_holder_get_track_key(struct TracksHolder *holder, const char *name);

/**
 * Return number of tracks in the holder.
 */
uintptr_t tracks_holder_count(const struct TracksHolder *holder);

#ifdef __cplusplus
}  // extern "C"
#endif  // __cplusplus

#ifdef __cplusplus
}  // namespace ffi
}  // namespace Tracks
#endif  // __cplusplus
