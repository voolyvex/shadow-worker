extends "res://scripts/base_object.gd"

signal pickaxe_collected(pickaxe)

func _ready():
	super._ready()
	add_to_group("tools")

func interact():
	pickaxe_collected.emit(self)
	queue_free() 