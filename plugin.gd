tool
extends EditorPlugin

const BAKE_METHOD_ID = 0
const CLEAR_METHOD_ID = 1

var _navigation_mesh_menu = null
var _selected_navigation_mesh = null


func _enter_tree():
	add_autoload_singleton(
		"AdvancedNavigationServer3D",
		"res://addons/godot-advanced-navigation-plugin/nodes/AdvancedNavigationServer3D.gdns"
	)
	add_custom_type(
		"AdvancedNavigationMesh3D",
		"Spatial",
		preload(
			"res://addons/godot-advanced-navigation-plugin/nodes/AdvancedNavigationMesh3D.gdns"
		),
		get_editor_interface().get_base_control().get_icon("Navigation", "EditorIcons")
	)
	add_custom_type(
		"AdvancedNavigationCrowd3D",
		"Node",
		preload(
			"res://addons/godot-advanced-navigation-plugin/nodes/AdvancedNavigationCrowd3D.gdns"
		),
		get_editor_interface().get_base_control().get_icon("Navigation", "EditorIcons")
	)
	add_custom_type(
		"AdvancedNavigationAgent3D",
		"Node",
		preload(
			"res://addons/godot-advanced-navigation-plugin/nodes/AdvancedNavigationAgent3D.gdns"
		),
		get_editor_interface().get_base_control().get_icon("NavigationAgent", "EditorIcons")
	)
	_navigation_mesh_menu = MenuButton.new()
	_navigation_mesh_menu.text = "Navigation mesh"
	_navigation_mesh_menu.get_popup().add_item("Bake", BAKE_METHOD_ID)
	_navigation_mesh_menu.get_popup().add_item("Clear", CLEAR_METHOD_ID)
	_navigation_mesh_menu.get_popup().connect(
		"id_pressed", self, "_on_navigation_mesh_menu_id_pressed"
	)
	add_control_to_container(CONTAINER_SPATIAL_EDITOR_MENU, _navigation_mesh_menu)
	_navigation_mesh_menu.hide()


func _exit_tree():
	remove_custom_type("AdvancedNavigationMesh3D")
	_navigation_mesh_menu.queue_free()


func handles(object):
	if object.script is NativeScript and object.has_method("bake"):
		_selected_navigation_mesh = object
		_navigation_mesh_menu.show()
	elif object is Node:
		_selected_navigation_mesh = null
		_navigation_mesh_menu.hide()


func _on_navigation_mesh_menu_id_pressed(id):
	match id:
		BAKE_METHOD_ID:
			_selected_navigation_mesh.bake()
		CLEAR_METHOD_ID:
			_selected_navigation_mesh.clear()
