extends "res://scripts/base_object.gd"

signal item_added(item)
signal item_removed(item)

var inventory = []
var max_items = 10
var is_open = false

func _ready():
    super._ready()
    set_interaction_text("Press E to open")

func _handle_interaction():
    is_open = !is_open
    if is_open:
        set_interaction_text("Press E to close")
        _show_inventory()
    else:
        set_interaction_text("Press E to open")
        _hide_inventory()

func add_item(item):
    if inventory.size() < max_items:
        inventory.append(item)
        emit_signal("item_added", item)
        return true
    return false

func remove_item(item):
    var index = inventory.find(item)
    if index != -1:
        inventory.remove_at(index)
        emit_signal("item_removed", item)
        return true
    return false

func has_item(item) -> bool:
    return item in inventory

func get_inventory() -> Array:
    return inventory.duplicate()

func is_full() -> bool:
    return inventory.size() >= max_items

func _show_inventory():
    # TODO: Implement inventory UI display
    pass

func _hide_inventory():
    # TODO: Implement inventory UI hiding
    pass 