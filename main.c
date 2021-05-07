#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char player1[20];
char player2[20];
#define WHITE    0
#define BLACK    1
#define EMPTY    2
#define PLAYABLE 3

#define WHITE_MARKER    "  W  "
#define BLACK_MARKER    "  B  "
#define EMPTY_MARKER    "     "
#define PLAYABLE_MARKER "  .  "

const char *row_names = "01234567";
const char *col_names = "abcdefgh";

char board[8][8];
int playable_direction[8][8][8];
int current_player;
int game_ended = 0;
int skipped_turn = 0;
int wrong_move = 0;
int has_valid_move = 0;
int scores[2];
int black_score = 2;

void init_game( )
{
    memset(board, EMPTY, sizeof( board ) );
    board[3][3] = BLACK;
    board[4][4] = BLACK;
    board[3][4] = WHITE;
    board[4][3] = WHITE;
    scores[WHITE] = 2;
    scores[BLACK] = 2;
    current_player = BLACK;
}

int is_valid_position( int i, int j )
{
    if ( i < 0 || i >= 8 || j < 0 || j >= 8 )
        return 0;
    else {
        return 1;
    }
}

int distance( int i1, int j1, int i2, int j2 )
{
    int di = abs( i1 - i2 ), dj = abs( j1 - j2 );
    if ( di > 0 ) return di;
    return dj;
}

int is_playable( int i, int j )
{
    memset( playable_direction[i][j], 0, 8 );
    if ( !is_valid_position( i, j ) ) return 0;
    if ( board[i][j] != EMPTY ) return 0;
    int playable = 0;

    int opposing_player = ( current_player + 1 ) % 2;

    // Test UL diagonal
    int i_it = i-1, j_it = j-1;
    while ( is_valid_position( i_it, j_it ) && board[i_it][j_it] == opposing_player )
    {
        i_it -= 1;
        j_it -= 1;
    }
    if ( is_valid_position( i_it, j_it ) && distance( i, j, i_it, j_it ) > 1 && board[i_it][j_it] == current_player )
    {
        playable_direction[i][j][0] = 1;
        playable = 1;
    }

    // Test UP path
    i_it = i-1, j_it = j;
    while ( is_valid_position( i_it, j_it ) && board[i_it][j_it] == opposing_player )
        i_it -= 1;

    if ( is_valid_position( i_it, j_it ) && distance( i, j, i_it, j_it ) > 1 && board[i_it][j_it] == current_player )
    {
        playable_direction[i][j][1] = 1;
        playable = 1;
    }

    // Test UR diagonal
    i_it = i-1, j_it = j+1;
    while ( is_valid_position( i_it, j_it ) && board[i_it][j_it] == opposing_player )
    {
        i_it -= 1;
        j_it += 1;
    }
    if ( is_valid_position( i_it, j_it ) && distance( i, j, i_it, j_it ) > 1 && board[i_it][j_it] == current_player )
    {
        playable_direction[i][j][2] = 1;
        playable = 1;
    }

    // Test LEFT path
    i_it = i, j_it = j-1;
    while ( is_valid_position( i_it, j_it ) && board[i_it][j_it] == opposing_player )
        j_it -= 1;

    if ( is_valid_position( i_it, j_it ) && distance( i, j, i_it, j_it ) > 1 && board[i_it][j_it] == current_player )
    {
        playable_direction[i][j][3] = 1;
        playable = 1;
    }

    // Test RIGHT path
    i_it = i, j_it = j+1;
    while ( is_valid_position( i_it, j_it ) && board[i_it][j_it] == opposing_player )
        j_it += 1;

    if ( is_valid_position( i_it, j_it ) && distance( i, j, i_it, j_it ) > 1 && board[i_it][j_it] == current_player )
    {
        playable_direction[i][j][4] = 1;
        playable = 1;
    }

    // Test DL diagonal
    i_it = i+1, j_it = j-1;
    while ( is_valid_position( i_it, j_it ) && board[i_it][j_it] == opposing_player )
    {
        i_it += 1;
        j_it -= 1;
    }
    if ( is_valid_position( i_it, j_it ) && distance( i, j, i_it, j_it ) > 1 && board[i_it][j_it] == current_player )
    {
        playable_direction[i][j][5] = 1;
        playable = 1;
    }

    // Test DOWN path
    i_it = i+1, j_it = j;
    while ( is_valid_position( i_it, j_it ) && board[i_it][j_it] == opposing_player )
        i_it += 1;

    if ( is_valid_position( i_it, j_it ) && distance( i, j, i_it, j_it ) > 1 && board[i_it][j_it] == current_player )
    {
        playable_direction[i][j][6] = 1;
        playable = 1;
    }

    // Test DR diagonal
    i_it = i+1, j_it = j+1;
    while ( is_valid_position( i_it, j_it ) && board[i_it][j_it] == opposing_player )
    {
        i_it += 1;
        j_it += 1;
    }
    if ( is_valid_position( i_it, j_it ) && distance( i, j, i_it, j_it ) > 1 && board[i_it][j_it] == current_player )
    {
        playable_direction[i][j][7] = 1;
        playable = 1;
    }
    return playable;
}

void mark_playable_positions( )
{
    has_valid_move = 0;
    for ( int i=0; i<8; ++i )
    {
        for ( int j=0; j<8; ++j )
        {
            if ( board[i][j] == PLAYABLE )
                board[i][j] = EMPTY;
            if ( is_playable( i, j ) )
            {
                board[i][j] = PLAYABLE;
                has_valid_move = 1;
            }
        }
    }
}

void draw_board( )
{
    printf( "     %c     %c     %c     %c     %c     %c     %c     %c\n", col_names[0], col_names[1], col_names[2], col_names[3], col_names[4], col_names[5], col_names[6], col_names[7] );
    printf( "   _____ _____ _____ _____ _____ _____ _____ _____\n" );
    for ( int i=0; i<8; ++i )
    {
        printf( "  |     |     |     |     |     |     |     |     |\n" );
        printf( "%c |", row_names[i] );
        for ( int j=0; j<8; ++j )
        {
            if ( board[i][j] == WHITE )
            {
                printf( "%s", WHITE_MARKER );
            } else if ( board[i][j] == BLACK )
            {
                printf( "%s", BLACK_MARKER );
            } else if ( board[i][j] == PLAYABLE )
            {
                printf( "%s", PLAYABLE_MARKER );
            } else
            {
                printf( "%s", EMPTY_MARKER );
            }
            printf("|");
        }
        printf( "\n" );
        printf( "  |_____|_____|_____|_____|_____|_____|_____|_____|\n" );
    }
    printf( "\n" );
}

void display_wrong_move( )
{
    if ( wrong_move )
    {
        printf( "You entered an invalid move! Please enter again\n" );
        wrong_move = 0;
    }
}

void display_current_player( )
{
    printf("The current player is:" );
    if ( current_player == WHITE )
        printf( "%s", player2 );
    else
        printf( "%s", player1 );
    printf( "\n" );
}

void change_current_player( )
{
    current_player = (current_player + 1 ) % 2;
}

void prompt_move( int *p_row, int *p_column )
{
    printf("Enter row [0-7] and column [0-7] separated by a single space.\n" );
    printf("The numbers hav to be between 0 and 7 for example 4 1.\n");
    scanf( "%d %d", p_row, p_column );
}

void capture_pieces( int i, int j )
{
    int opposing_player = ( current_player + 1 ) % 2;
    int i_it, j_it;

    // Capture UL diagonal
    if ( playable_direction[i][j][0] )
    {
        i_it = i-1, j_it = j-1;
        while ( board[i_it][j_it] == opposing_player )
        {
            board[i_it][j_it] = current_player;
            scores[current_player]++;
            scores[opposing_player]--;
            i_it -= 1;
            j_it -= 1;
        }
    }

    // Capture UP path
    if ( playable_direction[i][j][1] )
    {
        i_it = i-1, j_it = j;
        while ( board[i_it][j_it] == opposing_player )
        {
            board[i_it][j_it] = current_player;
            scores[current_player]++;
            scores[opposing_player]--;
            i_it -= 1;
        }
    }

    // Capture UR diagonal
    if ( playable_direction[i][j][2] )
    {
        i_it = i-1, j_it = j+1;
        while ( board[i_it][j_it] == opposing_player )
        {
            board[i_it][j_it] = current_player;
            scores[current_player]++;
            scores[opposing_player]--;
            i_it -= 1;
            j_it += 1;
        }
    }

    // Capture LEFT path
    if ( playable_direction[i][j][3] )
    {
        i_it = i, j_it = j-1;
        while ( board[i_it][j_it] == opposing_player )
        {
            board[i_it][j_it] = current_player;
            scores[current_player]++;
            scores[opposing_player]--;
            j_it -= 1;
        }
    }

    // Capture RIGHT path
    if ( playable_direction[i][j][4] )
    {
        i_it = i, j_it = j+1;
        while ( board[i_it][j_it] == opposing_player )
        {
            board[i_it][j_it] = current_player;
            scores[current_player]++;
            scores[opposing_player]--;
            j_it += 1;
        }
    }

    // Capture DL diagonal
    if ( playable_direction[i][j][5] )
    {
        i_it = i+1, j_it = j-1;
        while ( board[i_it][j_it] == opposing_player )
        {
            board[i_it][j_it] = current_player;
            scores[current_player]++;
            scores[opposing_player]--;
            i_it += 1;
            j_it -= 1;
        }
    }

    // Capture DOWN path
    if ( playable_direction[i][j][6] )
    {
        i_it = i+1, j_it = j;
        while ( board[i_it][j_it] == opposing_player )
        {
            board[i_it][j_it] = current_player;
            scores[current_player]++;
            scores[opposing_player]--;
            i_it += 1;
        }
    }

    // Capture DR diagonal
    if ( playable_direction[i][j][7] )
    {
        i_it = i+1, j_it = j+1;
        while ( board[i_it][j_it] == opposing_player )
        {
            board[i_it][j_it] = current_player;
            scores[current_player]++;
            scores[opposing_player]--;
            i_it += 1;
            j_it += 1;
        }
    }
}

void make_next_move( )
{
    int row, column;
    prompt_move( &row, &column );
    if ( is_valid_position( row, column ) && board[row][column] == PLAYABLE )
    {
        board[row][column] = current_player;
        scores[current_player]++;
        capture_pieces( row, column );
        change_current_player(  );
    }
    else wrong_move = 1;
}

void display_winner( )
{
    printf( "Final score:\n%s: %d %s: %d\n", WHITE_MARKER, scores[WHITE], BLACK_MARKER, scores[BLACK] );
    if ( scores[WHITE] > scores[BLACK] )
        printf( " Congratulations %s wins!!\n", player2 );
    else if ( scores[WHITE] < scores[BLACK] )
        printf( " Congratulations %s wins!!\n", player1 );
    else
        printf( "It's a draw.\n" );
}

void display_score( )
{
    printf( "%s: %d %s: %d\n", player1, scores[BLACK], player2, scores[WHITE]);
}
void file( ) {
    FILE *fp;
    fp = fopen("othello-results.txt", "w");
    fprintf(fp, "Final score:\n%s: %d %s: %d\n", WHITE_MARKER, scores[WHITE], BLACK_MARKER, scores[BLACK]);
    if (scores[WHITE] > scores[BLACK])
        printf(" Congratulations %s wins!!\n", player2);
    else if (scores[WHITE] < scores[BLACK])
        printf(" Congratulations %s wins!!\n", player1);
    else
        printf("It's a draw.\n");
}
int main( )
{
    printf("Enter the name of player1(Black): ");
    scanf("%s", player1);
    printf("Enter the name of the second player2(White): ");
    scanf("%s", player2);
    printf("Players are %s and %s\n", player1, player2);
    init_game();
    while (!game_ended ){
        if ( !wrong_move ) mark_playable_positions( );
        if ( !has_valid_move )
        {
            if ( skipped_turn )
            {
                game_ended = 1;
                draw_board( );
                continue;
            }
            skipped_turn = 1;
            change_current_player( );
            continue;
        }
        skipped_turn = 0;
        draw_board( );
        display_score( );
        display_current_player( );
        display_wrong_move( );
        make_next_move( );
    }
    display_winner( );
    file();
}
