extends Node

enum LOG_LEVEL {DEBUG, INFO, WARNING, ERROR}
var current_level = LOG_LEVEL.INFO
var log_file = null

func _ready():
    log_file = FileAccess.open("res://test_results.log", FileAccess.WRITE)
    if log_file:
        log_file.store_string("=== Test Run Started at " + Time.get_datetime_string_from_system() + " ===\n\n")

func _exit_tree():
    if log_file:
        log_file.store_string("\n=== Test Run Completed at " + Time.get_datetime_string_from_system() + " ===")
        log_file.close()

func debug(message: String):
    if current_level <= LOG_LEVEL.DEBUG:
        _write_log("[DEBUG] " + message)

func info(message: String):
    if current_level <= LOG_LEVEL.INFO:
        _write_log("[INFO] " + message)

func warning(message: String):
    if current_level <= LOG_LEVEL.WARNING:
        _write_log("[WARNING] " + message)

func error(message: String):
    if current_level <= LOG_LEVEL.ERROR:
        _write_log("[ERROR] " + message)

func _write_log(message: String):
    var timestamp = Time.get_datetime_string_from_system()
    var formatted_message = timestamp + " " + message + "\n"
    
    print(formatted_message)
    if log_file:
        log_file.store_string(formatted_message)

func set_level(level: LOG_LEVEL):
    current_level = level

func get_level() -> LOG_LEVEL:
    return current_level 