extends TileMap

const COLS : int = 6
const ROWS : int = 15
const BLANK := Vector2i(-1,-1)

# Movement
const directions := [Vector2i.LEFT, Vector2i.RIGHT, Vector2i.DOWN]
var steps : Array
var steps_req : int = 50
const start_pos := Vector2i(0,1)
var cur_pos : Vector2i
var speed: float

var score : int
const REWARD_H : int = 100
const REWARD_V : int = 150
const REWARD_D : int = 50
var game_running : bool

var tile_id : int = 0

var atlases = [0,0,0]

var atlas_0 : int

var board_layer : int = 0
var active_layer : int = 1

var piece := [Vector2i(2,0),Vector2i(2,1),Vector2i(2,2)]

var gems := [0,1,2,3,4,5]
var gems_full := gems.duplicate()

var tileset = 0

func  _ready() -> void:
	new_game()
	
func _process(delta: float) -> void:
	if Input.is_action_pressed("ui_down"):
		steps[2] += 10
	elif Input.is_action_pressed("ui_left"):
		steps[0] += 10
	elif Input.is_action_pressed("ui_right"):
		steps[1] += 10
	elif Input.is_action_just_pressed("ui_up"):
		cycle_piece()
	elif Input.is_action_just_pressed("ui_restart"):
		if not game_running:
			game_running = true
			new_game()
	# apply downward movement every frame
	steps[2] += speed
	# Move the piece
	for i in range(steps.size()):
		if steps[i] > steps_req:
			move_piece(directions[i])
			steps[i] = 0

func new_game() -> void:
	score = 0
	speed = 1.0
	game_running = true
	steps = [0,0,0]
	clear_board()
	$HUD.get_node("GameOverLabel").hide()
	$HUD.get_node("NewGameLabel").hide()
	create_piece()

func clear_board():
	for col in range(COLS - 1, 0, -1):
		for row in range(ROWS - 1, 0, -1):
			erase_cell(board_layer, Vector2i(col, row))

func create_piece():
	
	steps = [0,0,0]
	cur_pos = start_pos
	for i in range(3):
		atlases[i] = pick_random_gem()
	draw_piece(piece, cur_pos, atlases)

func clear_piece():
	for i in piece:
		erase_cell(active_layer, cur_pos + i)

func draw_piece(piece, pos, atlases):
	for i in range(3):
		var the_gem = Vector2i(atlases[i], tileset)
		set_cell(active_layer, pos + piece[i], tile_id, the_gem)

func cycle_piece():
	atlas_0 = atlases[0]
	atlases[0] = atlases[2]
	atlases[2] = atlases[1]
	atlases[1] = atlas_0
	draw_piece(piece, cur_pos, atlases)

func can_move(dir):
	var cm = true
	for i in piece:
		if not is_free(i + cur_pos + dir):
			cm = false
	return cm

func is_free(pos):
	return get_cell_source_id(board_layer, pos) == -1

func pick_random_gem() -> int:
	var gem
	if not gems.is_empty():
		gems.shuffle()
		gem = gems.pop_front()
	else:
		gems = gems_full.duplicate()
		gems.shuffle()
		gem = gems.pop_front()
	return gem

func move_piece(dir):
	if game_running:
		if can_move(dir):
			clear_piece()
			cur_pos += dir
			draw_piece(piece, cur_pos, atlases)
		else:
			if dir == Vector2i.DOWN:
				land_piece()
				check_board()
				create_piece()
				check_game_over()

func land_piece():
	for i in range(3):
		erase_cell(active_layer, cur_pos + piece[i])
		set_cell(board_layer, cur_pos + piece[i], tile_id, Vector2i(atlases[i], tileset))

func check_board():
	# Check for matches of 3 across the entire board
	
	var matches: int = 0
	
	# Horizontal check
	for col in range(COLS - 1, 0, -1):
		for row in range(ROWS - 1, 0, -1):
			if get_cell_atlas_coords(board_layer, Vector2i(col, row)) != BLANK and get_cell_atlas_coords(board_layer, Vector2i(col, row)) == get_cell_atlas_coords(board_layer, Vector2i(col - 1, row)):
				if get_cell_atlas_coords(board_layer, Vector2i(col - 1, row)) == get_cell_atlas_coords(board_layer, Vector2i(col - 2, row)):
					erase_cell(board_layer, Vector2i(col, row))
					erase_cell(board_layer, Vector2i(col - 1, row))
					erase_cell(board_layer, Vector2i(col - 2, row))
					matches += 1
					update_score("Horizontal")

	# Vertical check
	for col in range(COLS - 1, 0, -1):
		for row in range(ROWS - 1, 0, -1):
			if get_cell_atlas_coords(board_layer, Vector2i(col, row)) != BLANK and get_cell_atlas_coords(board_layer, Vector2i(col, row)) == get_cell_atlas_coords(board_layer, Vector2i(col, row - 1)):
				if get_cell_atlas_coords(board_layer, Vector2i(col, row - 1)) == get_cell_atlas_coords(board_layer, Vector2i(col, row - 2)):
					erase_cell(board_layer, Vector2i(col, row))
					erase_cell(board_layer, Vector2i(col, row - 1))
					erase_cell(board_layer, Vector2i(col, row - 2))
					matches += 1
					update_score("Vertical")

	# Diagonal check
	for col in range(COLS - 1, 0, -1):
		for row in range(ROWS - 1, 0, -1):
			if get_cell_atlas_coords(board_layer, Vector2i(col, row)) != BLANK and get_cell_atlas_coords(board_layer, Vector2i(col, row)) == get_cell_atlas_coords(board_layer, Vector2i(col - 1, row - 1)):
				if get_cell_atlas_coords(board_layer, Vector2i(col - 1, row - 1)) == get_cell_atlas_coords(board_layer, Vector2i(col - 2, row - 2)):
					erase_cell(board_layer, Vector2i(col, row))
					erase_cell(board_layer, Vector2i(col - 1, row - 1))
					erase_cell(board_layer, Vector2i(col - 2, row - 2))
					matches += 1
					update_score("Diagonal")
	
	# Diagonal check (alternatve)
	for col in range(COLS - 1, 0, -1):
		for row in range(ROWS - 1, 0, -1):
			if get_cell_atlas_coords(board_layer, Vector2i(col, row)) != BLANK and get_cell_atlas_coords(board_layer, Vector2i(col, row)) == get_cell_atlas_coords(board_layer, Vector2i(col - 1, row + 1)):
				if get_cell_atlas_coords(board_layer, Vector2i(col - 1, row + 1)) == get_cell_atlas_coords(board_layer, Vector2i(col - 2, row + 2)):
					erase_cell(board_layer, Vector2i(col, row))
					erase_cell(board_layer, Vector2i(col - 1, row + 1))
					erase_cell(board_layer, Vector2i(col - 2, row + 2))
					matches += 1
					update_score("Diagonal")

	for col in range(COLS - 1, -1, -1):
		for row in range (ROWS - 1, 0, -1):
			if get_cell_atlas_coords(board_layer, Vector2i(col, row)) != BLANK and get_cell_atlas_coords(board_layer, Vector2i(col, row + 1)) == BLANK:
				set_cell(board_layer, Vector2i(col, row + 1), tile_id, get_cell_atlas_coords(board_layer, Vector2i(col, row)))
				erase_cell(board_layer, Vector2i(col, row))

	if matches != 0:
		check_board()

func check_game_over():
	for i in piece:
		if not is_free(i + cur_pos):
			land_piece()
			$HUD.get_node("GameOverLabel").show()
			$HUD.get_node("NewGameLabel").show()
			game_running = false

# Add score based on the direction of the match of three
func update_score(dir: String):
	match dir:
		"Horizontal":
			score += REWARD_H
		"Vertical":
			score += REWARD_V
		"Diagonal":
			score += REWARD_D
	$HUD.get_node("ScoreLabel").text = "SCORE: " + str(score)
