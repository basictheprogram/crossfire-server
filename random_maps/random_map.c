/*
 * static char *rcsid_random_map_c =
 *   "$Id$";
 */

/*
    CrossFire, A Multiplayer game for X-windows

    Copyright (C) 1994 Mark Wedel
    Copyright (C) 1992 Frank Tore Johansen

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

    The author can be reached via e-mail to mark@pyramid.com
*/

#include <time.h>
#include <stdio.h>
#include <global.h>
#include <maze_gen.h>
#include <room_gen.h>
#include <random_map.h>
#include <rproto.h>
#include <sproto.h>

#define LO_NEWFILE 2



int Xsize;
int Ysize;

char wallstyle[512];
char wall_name[512];
char floorstyle[512];
char monsterstyle[512];
char treasurestyle[512];
char layoutstyle[512];
char decorstyle[512];
char doorstyle[512];
char exitstyle[512];
char final_map[512];
char origin_map[512];
char this_map[HUGE_BUF];

int layoutoptions1=0;
int layoutoptions2=0;
int layoutoptions3=0;
int symmetry=0;
int difficulty=0;
int difficulty_given=0;
int dungeon_level=0;
int dungeon_depth=0;
int decoroptions=0;
int orientation=0;
int origin_x=0;
int origin_y=0;
int random_seed=0;
long unsigned int total_map_hp=0;
int map_layout_style=0;
int treasureoptions=0;
int symmetry_used=0;
int generate_treasure_now=1; /* assume memory operation */

EXTERN FILE *logfile;
mapstruct *generate_random_map(char *InFileName,char *OutFileName) {
  FILE *InFile;
  char **layout;
  mapstruct *theMap;

  /* set up the random numbers */

  Xsize=-1;Ysize=-1;
  exitstyle[0]=final_map[0]=doorstyle[0]=decorstyle[0]=0;
  wallstyle[0]=floorstyle[0]=monsterstyle[0]=treasurestyle[0]=layoutstyle[0]=0;

  treasureoptions=0;
  layoutoptions1=0;
  layoutoptions2=0;
  layoutoptions3=0;
  symmetry=0;
  difficulty=0;
  dungeon_level=0;
  dungeon_depth=0;
  decoroptions=0;
  orientation=0;
  origin_x=0;
  origin_y=0;
  random_seed=0;
  total_map_hp=0;
  map_layout_style=0;
  symmetry_used=0;
  difficulty_given=0;

  /*  strcpy(this_map,OutFileName);
		sprintf(OutFilePath,"%s/maps%s",LIBDIR,OutFileName); */
  
  if((InFile=fopen(InFileName,"r"))==NULL) {
    printf("\nError:  can't open %s\n",InFileName);
    return(0);
  } 
  /*  if((OutFile=fopen(OutFilePath,"wb"))==NULL) {
    printf("\nError:  can't open %s\n",OutFilePath);
    return(0);
	 } */
  load_parameters(InFile,LO_NEWFILE);

  /* pick a random seed, or use the one from the input file */
  if(random_seed == 0)    SRANDOM(time(0));
  else SRANDOM(random_seed);

  if(difficulty==0)
    difficulty = dungeon_level; /* use this instead of a map difficulty  */
  else
	 difficulty_given=1;

  layout = layoutgen();

  /* increment these for the current map */
  dungeon_level+=1;
  /* allow constant-difficulty maps. */
/*  difficulty+=1; */

  /*  rotate the layout randomly */
  layout=rotate_layout(layout,RANDOM()%4);

  /* allocate the map and set the floor */
  theMap = make_map_floor(layout,floorstyle); 

  /* set the name of the map. */
  strcpy(theMap->path,OutFileName);

  make_map_walls(theMap,layout,wallstyle);

  put_doors(theMap,layout,doorstyle);

  place_exits(theMap,layout,exitstyle,orientation);

  place_monsters(theMap,monsterstyle,difficulty);

  place_specials_in_map(theMap,layout);

  /* treasures needs to have a proper difficulty set for
     the map. */
  theMap->difficulty=calculate_difficulty(theMap);

  place_treasure(theMap,layout,treasurestyle,treasureoptions);

  put_decor(theMap,layout,decorstyle,0);

  /* generate treasures, etc. */
  if(generate_treasure_now)
    fix_auto_apply(theMap);

  /* print a schematic of the maze */
  /*  for(i=0;i<Xsize;i++) {
		for(j=0;j<Ysize;j++) {
      if(layout[i][j]==0) layout[i][j]=' ';
      if(layout[i][j]=='*') layout[i][j]='D';
      printf("%c",layout[i][j]);
		}
		printf("\n");
		}*/

  fclose(InFile);
  /*  fclose(OutFile); */
  /*new_save_map(theMap,1);*/
  return theMap;
}


/*  function selects the layout function and gives it whatever
    arguments it needs.  */
char **layoutgen() {
  char **maze=0;
  if(Xsize<10) Xsize = 15 + RANDOM()%25;
  if(Ysize<10) Ysize = 15 + RANDOM()%25;
  if(symmetry == RANDOM_SYM) {
    symmetry_used = (RANDOM() % ( XY_SYM))+1;
    if(symmetry_used==Y_SYM||symmetry_used==XY_SYM) Ysize = Ysize/2+1;
	 if(symmetry_used==X_SYM||symmetry_used==XY_SYM) Xsize = Xsize/2+1;
  }
  else symmetry_used = symmetry;

  if(symmetry==Y_SYM||symmetry==XY_SYM) Ysize = Ysize/2+1;
  if(symmetry==X_SYM||symmetry==XY_SYM) Xsize = Xsize/2+1;

  if(strstr(layoutstyle,"onion")) {
    maze = map_gen_onion(Xsize,Ysize,layoutoptions1,layoutoptions2);
    if(!(RANDOM()%3)&& !(layoutoptions1 & OPT_WALLS_ONLY)) roomify_layout(maze);
  }

  if(strstr(layoutstyle,"maze")) {
    maze = maze_gen(Xsize,Ysize,layoutoptions1);
    if(!(RANDOM()%2)) doorify_layout(maze);
  }

  if(maze == 0) /* unknown or unspecified layout type, pick one at random */
    switch(RANDOM()%NROFLAYOUTS) {
    case 0:
      maze = maze_gen(Xsize,Ysize,RANDOM()%2);
      if(!(RANDOM()%2)) doorify_layout(maze);
      break;
    case 1:
      maze = map_gen_onion(Xsize,Ysize,layoutoptions1,layoutoptions2);
      if(!(RANDOM()%3)&& !(layoutoptions1 & OPT_WALLS_ONLY)) roomify_layout(maze);
      break;
    }

  maze = symmetrize_layout(maze, symmetry_used);

  return maze; 
}


/*  takes a map and makes it symmetric:  adjusts Xsize and
Ysize to produce a symmetric map. */

char **symmetrize_layout(char **maze, int sym) {
  int i,j;
  char **sym_maze;
  int Xsize_orig,Ysize_orig;
  Xsize_orig = Xsize;
  Ysize_orig = Ysize;
  symmetry_used = sym;  /* tell everyone else what sort of symmetry is used.*/
  if(sym == NO_SYM) {
	 Xsize = Xsize_orig;
	 Ysize = Ysize_orig;
	 return maze;
  }
  /* pick new sizes */
  Xsize = ((sym==X_SYM||sym==XY_SYM)?Xsize*2-3:Xsize);
  Ysize = ((sym==Y_SYM||sym==XY_SYM)?Ysize*2-3:Ysize);

  sym_maze = (char **)calloc(sizeof(char*),Xsize);
  for(i=0;i<Xsize;i++)
    sym_maze[i] = (char *)calloc(sizeof(char),Ysize);

  if(sym==X_SYM)
    for(i=0;i<Xsize/2+1;i++)
      for(j=0;j<Ysize;j++) {
		  sym_maze[i][j] = maze[i][j];
		  sym_maze[Xsize - i-1][j] = maze[i][j];
      };
  if(sym==Y_SYM)
    for(i=0;i<Xsize;i++)
      for(j=0;j<Ysize/2+1;j++) {
		  sym_maze[i][j] = maze[i][j];
		  sym_maze[i][Ysize-j-1] = maze[i][j];
      }
  if(sym==XY_SYM)
    for(i=0;i<Xsize/2+1;i++)
      for(j=0;j<Ysize/2+1;j++) {
		  sym_maze[i][j] = maze[i][j];
		  sym_maze[i][Ysize-j-1] = maze[i][j];
		  sym_maze[Xsize - i-1][j] = maze[i][j];
		  sym_maze[Xsize - i-1][Ysize-j-1] = maze[i][j];
      }
  /* delete the old maze */
  for(i=0;i<Xsize_orig;i++)
    free(maze[i]);
  free(maze);
  return sym_maze;
}


/*  takes  a map and rotates it.  This completes the
    onion layouts, making them possibly centered on any wall. 
    It'll modify Xsize and Ysize if they're swapped.
*/

char ** rotate_layout(char **maze,int rotation) {
  char **new_maze;
  int i,j;

  switch(rotation) {
  case 0:
    return maze;
    break;
  case 2:  /* a reflection */
    {
      char *new=malloc(sizeof(char) * Xsize*Ysize);
      for(i=0;i<Xsize;i++) {  /* make a copy */
		  for(j=0;j<Ysize;j++) {
			 new[i * Ysize + j] = maze[i][j];
		  }
      }
      for(i=0;i<Xsize;i++) { /* copy a reflection back */
		  for(j=0;j<Ysize;j++) {
			 maze[i][j]= new[(Xsize-i-1)*Ysize + Ysize-j-1];
		  }
      }
      return maze;
      break;
    }
  case 1:
  case 3:
    { int swap;
    new_maze = (char **) calloc(sizeof(char *),Ysize);
    for(i=0;i<Ysize;i++) {
      new_maze[i] = (char *) calloc(sizeof(char),Xsize);
    }
    if(rotation == 1) /* swap x and y */
      for(i=0;i<Xsize;i++)
		  for(j=0;j<Ysize;j++)
			 new_maze[j][i] = maze[i][j];

    if(rotation == 3) { /* swap x and y */
      for(i=0;i<Xsize;i++)
		  for(j=0;j<Ysize;j++)
			 new_maze[j][i] = maze[Xsize -i-1][Ysize - j-1];
    }

    /* delete the old layout */
    for(i=0;i<Xsize;i++)
      free(maze[i]);
    free(maze);

    swap = Ysize;
    Ysize = Xsize;
    Xsize = swap;
    return new_maze;
    break;
    }
  }
  return NULL;
}

/*  take a layout and make some rooms in it. 
    --works best on onions.*/
void roomify_layout(char **maze) {
  int tries = Xsize*Ysize/30;
  int ti;

  for(ti=0;ti<tries;ti++) {
    int dx,dy;  /* starting location for looking at creating a door */
    int cx,cy;  /* results of checking on creating walls. */
    dx = RANDOM() % Xsize;
    dy = RANDOM() % Ysize;
    cx = can_make_wall(maze,dx,dy,0);  /* horizontal */
    cy = can_make_wall(maze,dx,dy,1);  /* vertical */
    if(cx == -1) {
      if(cy != -1)
		  make_wall(maze,dx,dy,1);
      continue;
    }
    if(cy == -1) {
      make_wall(maze,dx,dy,0);
      continue;
    }
    if(cx < cy) make_wall(maze,dx,dy,0);
    else make_wall(maze,dx,dy,1);
    continue;
  }
}

/*  checks the layout to see if I can stick a horizontal(dir = 0) wall
    (or vertical, dir == 1)
    here which ends up on other walls sensibly.  */
    
int can_make_wall(char **maze,int dx,int dy,int dir) {
  int i1;
  int length=0;

  /* dont make walls if we're on the edge. */
  if(dx == 0 || dx == (Xsize -1) || dy == 0 || dy == (Ysize-1)) return -1;

  /* don't make walls if we're ON a wall. */
  if(maze[dx][dy]!=0) return -1;

  if(dir==0) /* horizontal */
    {
      int y = dy;
      for(i1=dx-1;i1>0;i1--) {
		  int sindex=surround_flag(maze,i1,y);
		  if(sindex == 1) break;  
		  if(sindex != 0) return -1;  /* can't make horiz.  wall here */
		  length++;
      }
	
      for(i1=dx+1;i1<Xsize-1;i1++) {
		  int sindex=surround_flag(maze,i1,y);
		  if(sindex == 2) break;  
		  if(sindex != 0) return -1;  /* can't make horiz.  wall here */
		  length++;
      }
      return length;
    }
  else {  /* vertical */
	 int x = dx;
	 for(i1=dy-1;i1>0;i1--) {
		int sindex=surround_flag(maze,x,i1);
		if(sindex == 4) break;  
		if(sindex != 0) return -1;  /* can't make vert. wall here */
		length++;
	 }
	
	 for(i1=dy+1;i1<Ysize-1;i1++) {
		int sindex=surround_flag(maze,x,i1);
		if(sindex == 8) break;  
		if(sindex != 0) return -1;  /* can't make verti. wall here */
		length++;
	 }
	 return length;
  }
  return -1;   
}


int make_wall(char **maze,int x, int y, int dir){
  maze[x][y] = 'd'; /* mark a door */
  switch(dir) {
  case 0: /* horizontal */
    {
      int i1;
      for(i1 = x-1;maze[i1][y]==0;i1--) 
		  maze[i1][y]='#';
      for(i1 = x+1;maze[i1][y]==0;i1++)
		  maze[i1][y]='#';
      break;
    }
  case 1: /* vertical */
    {
      int i1;
      for(i1 = y-1;maze[x][i1]==0;i1--) 
		  maze[x][i1]='#';
      for(i1 = y+1;maze[x][i1]==0;i1++)
		  maze[x][i1]='#';
      break;
    }
  }      

  return 0;
}

/*  puts doors at appropriate locations in a layout. */

void doorify_layout(char **maze) {
  int ndoors = Xsize*Ysize/60;  /* reasonable number of doors. */
  int *doorlist_x;
  int *doorlist_y;
  int doorlocs = 0;  /* # of available doorlocations */
  int i,j;
  
  doorlist_x = malloc(sizeof(int) * Xsize*Ysize);
  doorlist_y = malloc(sizeof(int) * Xsize*Ysize);


  /* make a list of possible door locations */
  for(i=1;i<Xsize-1;i++)
    for(j=1;j<Ysize-1;j++) {
      int sindex = surround_flag(maze,i,j);
      if(sindex == 3 || sindex == 12) /* these are possible door sindex*/
		  {
			 doorlist_x[doorlocs]=i;
			 doorlist_y[doorlocs]=j;
			 doorlocs++;
		  }
    }
  while(ndoors > 0 && doorlocs > 0) {
    int di;
    int sindex;
    di = RANDOM() % doorlocs;
    i=doorlist_x[di];
    j=doorlist_y[di];
    sindex= surround_flag(maze,i,j);
    if(sindex == 3 || sindex == 12) /* these are possible door sindex*/
      {
		  maze[i][j] = '*';
		  ndoors--;
      }
    /* reduce the size of the list */
    doorlocs--;
    doorlist_x[di]=doorlist_x[doorlocs];
    doorlist_y[di]=doorlist_y[doorlocs];
  }
}


void write_map_parameters_to_string(char *buf) {

  char small_buf[256];
  sprintf(buf,"xsize %d\nysize %d\n",Xsize,Ysize);

  if(wallstyle[0]) {
    sprintf(small_buf,"wallstyle %s\n",wallstyle);
    strcat(buf,small_buf);
  }

  if(floorstyle[0]) {
    sprintf(small_buf,"floorstyle %s\n",floorstyle);
    strcat(buf,small_buf);
  }

  if(monsterstyle[0]) {
    sprintf(small_buf,"monsterstyle %s\n",monsterstyle);
    strcat(buf,small_buf);
  }

  if(treasurestyle[0]) {
    sprintf(small_buf,"treasurestyle %s\n",treasurestyle);
    strcat(buf,small_buf);
  }

  if(layoutstyle[0]) {
    sprintf(small_buf,"layoutstyle %s\n",layoutstyle);
    strcat(buf,small_buf);
  }

  if(decorstyle[0]) {
    sprintf(small_buf,"decorstyle %s\n",decorstyle);
    strcat(buf,small_buf);
  }

  if(doorstyle[0]) {
    sprintf(small_buf,"doorstyle %s\n",doorstyle);
    strcat(buf,small_buf);
  }

  if(exitstyle[0]) {
    sprintf(small_buf,"exitstyle %s\n",exitstyle);
    strcat(buf,small_buf);
  }

  if(final_map[0]) {
    sprintf(small_buf,"final_map %s\n",final_map);
    strcat(buf,small_buf);
  }

  if(this_map[0]) {
    sprintf(small_buf,"origin_map %s\n",this_map);
    strcat(buf,small_buf);
  }

  if(layoutoptions1) {
    sprintf(small_buf,"layoutoptions1 %d\n",layoutoptions1);
    strcat(buf,small_buf);
  }


  if(layoutoptions2) {
    sprintf(small_buf,"layoutoptions2 %d\n",layoutoptions2);
    strcat(buf,small_buf);
  }


  if(layoutoptions3) {
    sprintf(small_buf,"layoutoptions3 %d\n",layoutoptions3);
    strcat(buf,small_buf);
  }

  if(symmetry) {
    sprintf(small_buf,"symmetry %d\n",symmetry);
    strcat(buf,small_buf);
  }


  if(difficulty && difficulty_given ) {
    sprintf(small_buf,"difficulty %d\n",difficulty);
    strcat(buf,small_buf);
  }

  sprintf(small_buf,"dungeon_level %d\n",dungeon_level);
  strcat(buf,small_buf);

  if(dungeon_depth) {
    sprintf(small_buf,"dungeon_depth %d\n",dungeon_depth);
    strcat(buf,small_buf);
  }

  if(decoroptions) {
    sprintf(small_buf,"decoroptions %d\n",decoroptions);
    strcat(buf,small_buf);
  }

  if(orientation) {
    sprintf(small_buf,"orientation %d\n",orientation);
    strcat(buf,small_buf);
  }

  if(origin_x) {
    sprintf(small_buf,"origin_x %d\n",origin_x);
    strcat(buf,small_buf);
  }

  if(origin_y) {
    sprintf(small_buf,"origin_y %d\n",origin_y);
    strcat(buf,small_buf);
  }
  if(random_seed) {
    sprintf(small_buf,"random_seed %d\n",random_seed + 1);
    strcat(buf,small_buf);
  }

  if(treasureoptions) {
    sprintf(small_buf,"treasureoptions %d\n",treasureoptions);
    strcat(buf,small_buf);
  }


}
