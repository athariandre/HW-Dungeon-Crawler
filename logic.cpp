#include <iostream>
#include <fstream>
#include <string>
#include "logic.h"

using std::cout, std::endl, std::ifstream, std::string;

/**
 * TODO: Student implement this function
 * Load representation of the dungeon level from file into the 2D map.
 * Calls createMap to allocate the 2D array.
 * @param   fileName    File name of dungeon level.
 * @param   maxRow      Number of rows in the dungeon table (aka height).
 * @param   maxCol      Number of columns in the dungeon table (aka width).
 * @param   player      Player object by reference to set starting position.
 * @return  pointer to 2D dynamic array representation of dungeon map with player's location., or nullptr if loading fails for any reason
 * @updates  maxRow, maxCol, player
 */
char** loadLevel(const string& fileName, int& maxRow, int& maxCol, Player& player) {

    ifstream myfile(fileName);


    myfile >> maxRow >> maxCol >> player.row >> player.col;
    

    if(myfile.fail()){
        return nullptr;
    }

    if(maxRow <= 0 || maxRow > 999999 || maxCol <= 0 || maxCol > 999999 || player.row > maxRow || player.row < 0 || player.col > maxCol || player.col < 0){
        return nullptr;
    }


    char** map = createMap(maxRow, maxCol);


    for(int i = 0; i < maxRow; i++){
        for(int j = 0; j < maxCol; j++){
            char val;
            myfile >> val;
        
            if(myfile.fail()){
                return nullptr;
            }
            if(val != TILE_OPEN && val != TILE_PLAYER && val != TILE_TREASURE && val != TILE_AMULET && val != TILE_MONSTER && val != TILE_PILLAR && val != TILE_DOOR && val != TILE_EXIT){
                return nullptr;
            }

            map[i][j] = val;
        }
    }

    map[player.row][player.col] = TILE_PLAYER;


    return map;
}

/**
 * TODO: Student implement this function
 * Translate the character direction input by the user into row or column change.
 * That is, updates the nextRow or nextCol according to the player's movement direction.
 * @param   input       Character input by the user which translates to a direction.
 * @param   nextRow     Player's next row on the dungeon map (up/down).
 * @param   nextCol     Player's next column on dungeon map (left/right).
 * @updates  nextRow, nextCol
 */
void getDirection(char input, int& nextRow, int& nextCol) {

    if(input != MOVE_UP && input != MOVE_DOWN && input != MOVE_LEFT && input != MOVE_RIGHT){
        return;
    }

    if(input == MOVE_UP){
        nextRow -= 1;
    }
    else if(input == MOVE_DOWN){
        nextRow += 1;
    }
    else if(input == MOVE_RIGHT){
        nextCol += 1;
    }
    else{
        nextCol -= 1;
    }

}

/**
 * TODO: [suggested] Student implement this function
 * Allocate the 2D map array.
 * Initialize each cell to TILE_OPEN.
 * @param   maxRow      Number of rows in the dungeon table (aka height).
 * @param   maxCol      Number of columns in the dungeon table (aka width).
 * @return  2D map array for the dungeon level, holds char type.
 */
char** createMap(int maxRow, int maxCol) {


    if(maxRow <= 0 || maxCol <= 0){
        return nullptr;
    }


    char** map = new char*[maxRow];


    for(int i = 0; i < maxRow; i++){
        map[i] = new char[maxCol];
        for(int j = 0; j < maxCol; j++){
            map[i][j] = TILE_OPEN;
        }
    }

    return map;
}

/**
 * TODO: Student implement this function
 * Deallocates the 2D map array.
 * @param   map         Dungeon map.
 * @param   maxRow      Number of rows in the dungeon table (aka height).
 * @return None
 * @update map, maxRow
 */
void deleteMap(char**& map, int& maxRow) {
    for(int i = 0; i < maxRow; i++){
        delete[] map[i];
    }
 delete[] map;
    map = nullptr;
    maxRow = 0;
}

/**
 * TODO: Student implement this function
 * Resize the 2D map by doubling both dimensions.
 * Copy the current map contents to the right, diagonal down, and below.
 * Do not duplicate the player, and remember to avoid memory leaks!
 * You can use the STATUS constants defined in logic.h to help!
 * @param   map         Dungeon map.
 * @param   maxRow      Number of rows in the dungeon table (aka height), to be doubled.
 * @param   maxCol      Number of columns in the dungeon table (aka width), to be doubled.
 * @return  pointer to a dynamically-allocated 2D array (map) that has twice as many columns and rows in size.
 * @update maxRow, maxCol
 */
char** resizeMap(char** map, int& maxRow, int& maxCol) {
    char**resizedMap = createMap(maxRow*2, maxCol*2);

    for(int i = 0; i < maxRow; i++){
        for(int j = 0; j < maxCol; j++){
            if(map[i][j] != TILE_PLAYER){
                cout << "resizing map, adding tile at i,j " << i << ", " << j << endl;
                resizedMap[i][j] = map[i][j];
                resizedMap[i+maxRow][j] = map[i][j];
                resizedMap[i][j+maxCol] = map[i][j];
                resizedMap[i+maxRow][j+maxCol] = map[i][j];
            }
            else{
                resizedMap[i][j] = map[i][j];
            }
        }
    }
    
    cout << "pre delete" << endl;

    int temp = maxRow;

    deleteMap(map, temp);

    cout << "post delete" << endl;

    cout << "pre dimension change row,col = " << maxRow << " " << maxCol << endl;

    maxRow *= 2;
    maxCol *= 2;

    cout << "dimensions changed to row,col = " << maxRow << " " << maxCol << endl;

    for(int i = 0; i < maxRow; i++){
        for(int j = 0; j < maxCol; j++){
            cout << resizedMap[i][j] << " ";
        }
        cout << endl;
    }
    
    return resizedMap;
}

/**
 * TODO: Student implement this function
 * Checks if the player can move in the specified direction and performs the move if so.
 * Cannot move out of bounds or onto TILE_PILLAR or TILE_MONSTER.
 * Cannot move onto TILE_EXIT without at least one treasure. 
 * If TILE_TREASURE, increment treasure by 1.
 * Remember to update the map tile that the player moves onto and return the appropriate status.
 * You can use the STATUS constants defined in logic.h to help!
 * @param   map         Dungeon map.
 * @param   maxRow      Number of rows in the dungeon table (aka height).
 * @param   maxCol      Number of columns in the dungeon table (aka width).
 * @param   player      Player object to by reference to see current location.
 * @param   nextRow     Player's next row on the dungeon map (up/down).
 * @param   nextCol     Player's next column on dungeon map (left/right).
 * @return  Player's movement status after updating player's position.
 * @update map contents, player
 */
int doPlayerMove(char** map, int maxRow, int maxCol, Player& player, int nextRow, int nextCol) {
    int status = 0;
    if(nextRow < 0 || nextRow >= maxRow || nextCol < 0 || nextCol >= maxCol || map[nextRow][nextCol] == TILE_MONSTER || map[nextRow][nextCol] == TILE_PILLAR){
        status = STATUS_STAY;
        nextRow = player.row;
        nextCol = player.col;
    }
    else if(map[nextRow][nextCol] == TILE_TREASURE){
        player.treasure += 1;
        status = STATUS_TREASURE;
    }
    else if(map[nextRow][nextCol] == TILE_AMULET){
        status = STATUS_AMULET;
    }
    else if(map[nextRow][nextCol] == TILE_DOOR){
        status = STATUS_LEAVE;
    }
    else if(map[nextRow][nextCol] == TILE_EXIT){
        if(player.treasure != 0){
            status = STATUS_ESCAPE;
        }  
        else{
            status = STATUS_STAY;
            nextRow = player.row;
            nextCol = player.col;
        }
    }
    else{
        status = STATUS_MOVE;
        
    }

    map[player.row][player.col] = TILE_OPEN;

    player.row = nextRow;
    player.col = nextCol;

    map[player.row][player.col] = TILE_PLAYER;

    return status;
}

/**
 * TODO: Student implement this function
 * Update monster locations:
 * We check up, down, left, right from the current player position.
 * If we see an obstacle, there is no line of sight in that direction, and the monster does not move.
 * If we see a monster before an obstacle, the monster moves one tile toward the player.
 * We should update the map as the monster moves.
 * At the end, we check if a monster has moved onto the player's tile.
 * @param   map         Dungeon map.
 * @param   maxRow      Number of rows in the dungeon table (aka height).
 * @param   maxCol      Number of columns in the dungeon table (aka width).
 * @param   player      Player object by reference for current location.
 * @return  Boolean value indicating player status: true if monster reaches the player, false if not.
 * @update map contents
 */
bool doMonsterAttack(char** map, int maxRow, int maxCol, const Player& player) {
    //checking right


    for(int i = player.col; i < maxCol; i++){
        if(i == player.col){
            continue;
        }
        if(map[player.row][i] == TILE_PILLAR){
            break;
        }
        if(map[player.row][i] == TILE_MONSTER){
            map[player.row][i] = TILE_OPEN;
            map[player.row][i-1] = TILE_MONSTER;
        }
    }

    //checking left
    for(int i = player.col; i >= 0; i--){
        if(i == player.col){
            continue;
        }
        if(map[player.row][i] == TILE_PILLAR){
            break;
        }
        if(map[player.row][i] == TILE_MONSTER){
            map[player.row][i] = TILE_OPEN;
            map[player.row][i+1] = TILE_MONSTER;
        }
    }


    //checking down
    for(int i = player.row; i < maxRow; i++){
        if(i == player.row){
            continue;
        }
        if(map[i][player.col] == TILE_PILLAR){
            break;
        }
        if(map[i][player.col] == TILE_MONSTER){
            map[i][player.col] = TILE_OPEN;
            map[i-1][player.col] = TILE_MONSTER;
        }
    }

    //checking up
    for(int i = player.row; i >= 0; i--){
        if(i == player.row){
            continue;
        }
        if(map[i][player.col] == TILE_PILLAR){
            break;
        }
        if(map[i][player.col] == TILE_MONSTER){
            map[i][player.col] = TILE_OPEN;
            map[i+1][player.col] = TILE_MONSTER;
        }
    }


    if(map[player.row][player.col] == TILE_MONSTER){
        return true;
    }
    return false;
}
