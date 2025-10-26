#ifndef GLOBAL_H
#define GLOBAL_H

extern char cur_file [MAX_NAME+EXTENTION];
extern int error;/*error flag*/
extern int ic, dc;/*instructions counter and data counter*/
extern int current_symbol_count;
extern int entries_count;
extern int externals_count;
extern int use_label_count;
extern int exit_flag;

#endif
