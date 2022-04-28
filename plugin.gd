tool
extends EditorPlugin


func _enter_tree():
	add_custom_type(
		"AdvancedNavigationMesh3D",
		"Spatial",
		preload("res://addons/godot-advanced-navigation-plugin/nodes/AdvancedNavigationMesh3D.gdns"),
		get_editor_interface().get_base_control().get_icon("Spatial", "EditorIcons")
	)


func _exit_tree():
	remove_custom_type("AdvancedNavigationMesh3D")
