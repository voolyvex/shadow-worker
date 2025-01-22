@tool
extends EditorPlugin

var asset_manager

func _enter_tree():
    asset_manager = preload("res://addons/asset_manager/asset_manager_dock.gd").new()
    add_control_to_dock(DOCK_SLOT_RIGHT_UL, asset_manager)
    print("Asset Manager plugin initialized")

func _exit_tree():
    if asset_manager:
        remove_control_from_docks(asset_manager)
        asset_manager.free()
    print("Asset Manager plugin cleaned up") 