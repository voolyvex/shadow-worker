extends Node2D

var debug_enabled = false
var font: Font

func _ready():
    font = ThemeDB.fallback_font
    visible = false

func _input(event):
    if event.is_action_pressed("toggle_debug"):
        debug_enabled = !debug_enabled
        visible = debug_enabled
        queue_redraw()

func _process(_delta):
    if debug_enabled:
        queue_redraw()

func _draw():
    if not debug_enabled:
        return
        
    var fps = Engine.get_frames_per_second()
    draw_string(font, Vector2(10, 20), "FPS: " + str(fps), HORIZONTAL_ALIGNMENT_LEFT)
    
    var player = get_node_or_null("/root/Main/Player")
    if player:
        var pos = player.position
        draw_string(font, Vector2(10, 40), "Player Pos: " + str(pos), HORIZONTAL_ALIGNMENT_LEFT)
        
        var ray = player.get_node_or_null("InteractionRay")
        if ray:
            draw_line(player.position, player.position + ray.target_position, Color.YELLOW)
            
    var tilemap = get_node_or_null("/root/Main/WorldSystem/TileMap") 
    if tilemap:
        var used = tilemap.get_used_cells(0)
        draw_string(font, Vector2(10, 60), "Tiles: " + str(used.size()), HORIZONTAL_ALIGNMENT_LEFT)
        
        var mouse_pos = get_viewport().get_mouse_position()
        var tile_pos = tilemap.local_to_map(tilemap.to_local(mouse_pos))
        draw_string(font, Vector2(10, 80), "Tile: " + str(tile_pos), HORIZONTAL_ALIGNMENT_LEFT)
        
    var reality = get_node_or_null("/root/Main/RealitySystem")
    if reality:
        var distortion = reality.get_global_distortion()
        draw_string(font, Vector2(10, 100), "Distortion: " + str(distortion), HORIZONTAL_ALIGNMENT_LEFT)
        
        var anomalies = reality.get_active_anomalies()
        for anomaly in anomalies:
            var pos = anomaly.position
            var radius = anomaly.radius
            draw_circle(pos, radius, Color(1, 0, 1, 0.2))
            # Draw circle outline using lines
            var segments = 32
            var angle_delta = 2 * PI / segments
            for i in range(segments):
                var start_angle = i * angle_delta
                var end_angle = (i + 1) * angle_delta
                var start_pos = pos + Vector2(cos(start_angle), sin(start_angle)) * radius
                var end_pos = pos + Vector2(cos(end_angle), sin(end_angle)) * radius
                draw_line(start_pos, end_pos, Color.MAGENTA) 