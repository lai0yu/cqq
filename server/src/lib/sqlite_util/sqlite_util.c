#include "sqlite_util.h"

int db_insert(const char *table_columns, const char *values) {
    bzero(fsql, sizeof(fsql));
    sprintf(fsql, insert_sql, table_columns, values);
    return exec_sql(fsql, NULL, NULL);
}

int db_delete(const char *table, const char *where) {
    bzero(fsql, sizeof(fsql));
    sprintf(fsql, delete_sql, table, where);
    return exec_sql(fsql, NULL, NULL);
}

struct select_row *db_select(const char *table, const char *columns, const char *where) {
    struct select_row *row_head = (struct select_row *)malloc(sizeof(struct select_row));
    INIT_LIST_HEAD(&row_head->list);
    row_head->index = 0;
    bzero(fsql, sizeof(fsql));
    sprintf(fsql, select_sql, columns, table, where);
    exec_sql(fsql, select_callback, row_head);
    return row_head;
}

int db_update(const char *table, const char *sets, const char *where) {
    bzero(fsql, sizeof(fsql));
    sprintf(fsql, update_sql, table, sets, where);
    return exec_sql(fsql, NULL, NULL);
}

void free_select_rows_list(struct select_row *row_head) {
    struct select_row *pos = NULL;

    int i;
    list_for_each_entry(pos, &(row_head->list), list) {
        if (pos == NULL) { continue; }
        if (pos->argv != NULL) {
            for (i = 0; i < pos->argc; i++) {
                if (pos->argv[i] == NULL) { continue; }
                free(pos->argv[i]);
                pos->argv[i] = NULL;
            }
            free(pos->argv);
            pos->argv = NULL;
        }
        if (pos->azColName != NULL) {
            for (i = 0; i < pos->argc; i++) {
                if (pos->azColName[i] == NULL) { continue; }
                free(pos->azColName[i]);
                pos->azColName[i] = NULL;
            }
            free(pos->azColName);
            pos->azColName = NULL;
        }
    }

    while (row_head->list.next != &row_head->list) {
        struct select_row *next_row = list_entry(row_head->list.next, struct select_row, list);
        list_del(row_head->list.next);
        free(next_row);
    }
    row_head = NULL;
}
