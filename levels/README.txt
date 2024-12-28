NOTE: Each lvl dir will have the original state of the lvl and it should be copied to and modified in the player's save file.


Structure of each of the lvl:
Data is to be encoded in json format only
1. timemap.json: Shall contain all the tiles that can/will exist in the lvl. This includes world_element, decoration, and non-intractable elements.
    The structure should be like:
    {
        "tile_name": {
            "asset_path": "path/to/asset/relative/to/build/Game",
            "tile_size": [w, h],
            "pos_matrix": [
                [x1_index, y1_index],
                .
                .
                .
                [xn_index, yn_index]
            ], // NOTE: This xn_index and yn_index are the (actual_tile_pos)/(tile_size). Made this way for ease of use.
            "properties": {
               "is_animated": 0/1, // This will mean that the asset path is the path to a sprite sheet. As these tiles are non-intractable they will only have 1 type of animation.
                "collision": 0/1
            }
        }
    }
    These tiles are assumed to be unbreakable/unmovable elements.