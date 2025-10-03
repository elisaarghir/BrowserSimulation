/*ARGHIR Elisa-Elena 312CC*/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

struct page {
    int id;
    char url[50];
    char *description;
};

struct Node {
    struct page *p;
    struct Node *next;
};

struct stack {
    struct Node *top;
};

struct tab {
    int id;
    struct page *currentPage;
    struct stack *backwardStack;
    struct stack *forwardStack;
};

struct tabsList {
    struct tab *t;
    struct tabsList *next, *prev;
};

struct browser {
    struct tab *current;
    struct tabsList *list;
};

/* functia tabs initializeaza elementele unui tab nou, 
cat si pagina implicita din acesta */

struct tab *tabs() 
{
    struct tab *t = malloc(sizeof(struct tab));
    int length;
    if (t == NULL) {
        return NULL;
    }

    t->id = 0;
    t->currentPage = malloc(sizeof(struct page));
    t->currentPage->id = 0;
    strcpy(t->currentPage->url, "https://acs.pub.ro/");
    length = strlen("Computer Science\n");
    t->currentPage->description = malloc((length + 1) * sizeof(char));
    if (t->currentPage->description == NULL) {
        return NULL;
    }
    strcpy(t->currentPage->description, "Computer Science\n");

    t->backwardStack = malloc(sizeof(struct stack));
    if (t->backwardStack == NULL) {
        return NULL;
    }
    t->backwardStack->top = NULL;
    t->forwardStack = malloc(sizeof(struct stack));
    if (t->forwardStack == NULL) {
        return NULL;
    }
    t->forwardStack->top = NULL;

    return t;
}

/*functie pentru initializarea unei structuri browser, cat si a listei 
in care creez santinela si primul nod (first), care contine tab cu id 0*/

struct browser *create_browser() 
{
    struct browser *b = malloc(sizeof(struct browser));
    struct tabsList *first = malloc(sizeof(struct tabsList));
    struct tab *t = tabs();

    if (b == NULL ||first == NULL) {
        return NULL;
    }
    b->list = malloc(sizeof(struct tabsList));
    if (b->list == NULL) {
        return NULL;
    }

    b->list->t = NULL;
    b->list->next = b->list;
    b->list->prev = b->list;
    first->t = t;
    first->next = b->list;
    first->prev = b->list->prev;
    b->list->prev->next = first;
    b->list->prev = first;
    b->current = t;

    return b;
}

//folosesc pointerul last_tab_id pentru a retine id-ul ultimului tab

void new_tab(struct browser *b, int *last_tab_id) 
{
    struct tabsList *new_node = malloc(sizeof(struct tabsList));
    struct tab *t = tabs();
    if (new_node == NULL || t == NULL) {
        return;
    }

    t->id = (*last_tab_id) + 1;
    new_node->t = t;
    new_node->next = b->list;
    new_node->prev = b->list->prev;
    b->list->prev->next = new_node;
    b->list->prev = new_node;
    b->current = t;
    (*last_tab_id)++;
}

/*folosesc pointerul q pentru a afisa lista circular pornind de la 
tab-ul urmator dupa current*/

void print(struct browser *b, FILE *f) 
{
    struct tabsList *p = b->list->next, *q = NULL;
    int ok = 0;
    if (b->current == NULL || p == b->list) {
        return;
    }

    while (p != b->list && ok == 0) {
        if (p->t == b->current) {
            ok = 1;
            break;
        }
        p = p->next;
    }
    if (p == b->list) {
        return;
    }

    fprintf(f, "%d", p->t->id);
    q = p->next;
    if (q == b->list) {
        q = q->next;
    }
    while (q != p) {
        fprintf(f, " %d", q->t->id);
        q = q->next;
        if (q == b->list) {
            q = q->next;
        }
    }

    fprintf(f, "\n");
    fprintf(f, "%s", b->current->currentPage->description);
}

//cu functia free_stack eliberez fiecare element al stivei

void free_stack(struct stack *s) 
{
    if (s == NULL) {
        return; 
    }
    struct Node *tmp;

    while (s->top != NULL) {
        tmp = s->top;
        s->top = s->top->next;
        free(tmp->p->description);
        free(tmp->p);
        free(tmp);
    }
}

//folosesc free_stack_completely pentru a elibera stiva de tot din memorie

void free_stack_completely(struct stack *s) 
{
    if (s == NULL) {
        return; 
    }
    struct Node *tmp;

    while (s->top != NULL) {
        tmp = s->top;
        s->top = s->top->next;
        free(tmp->p->description);
        free(tmp->p);
        free(tmp);
    }

    free(s);
}

//acutalizez legaturile din lista si dupa eliberez memoria

void close(struct browser *b, FILE *f) 
{
    struct tabsList *p = b->list->next;
    int ok = 0;

    while (p != b->list && ok == 0) {
        if (p->t == b->current) {
            ok = 1;
            break;
        }
        p = p->next;
    }
    if (p == b->list) {
        return;
    }
    if (p->t->id == 0) {
        fprintf(f, "403 Forbidden\n");
        return;
    }
    b->current = p->prev->t;
    p->prev->next = p->next;
    p->next->prev = p->prev;

    free(p->t->currentPage->description);
    free(p->t->currentPage);
    free_stack_completely(p->t->backwardStack);
    free_stack_completely(p->t->forwardStack);
    free(p->t);
    free(p);
}

//variabila ok retine daca exista un tab cu id-ul respectiv

void open(struct browser *b, int id, FILE *f) 
{
    struct tabsList *p = b->list->next;
    int ok = 0;

    while (p != b->list) {
        if (p->t->id == id)  {
            b->current = p->t;
            ok = 1;
        }
        p = p->next;
    }

    if (ok == 0) {
        fprintf(f, "403 Forbidden\n");
        return;
    }
}

//cu conditia din if trec peste santinela, daca tabul curent este ultimul

void next(struct browser *b) 
{
    struct tabsList *p = b->list->next;
    while (p != b->list && p->t != b->current) {
        p = p->next;
    }
    p = p->next;
    if (p == b->list) {
        p = p->next;
    }
    b->current = p->t;
}

//cu conditia din if trec peste santinela, daca tab-ul curent este primul

void prev(struct browser *b) 
{
    struct tabsList *p = b->list->next;

    while (p != b->list && p->t != b->current) {
        p = p->next;
    }
    p = p->prev;
    if (p == b->list) {
        p = p->prev;
    }

    b->current = p->t;
}

/*cu aceasta functie adaug o noua pagina in stiva, copiind fiecare element 
al ei intr-un nou nod*/

void add_to_stack(struct stack *s, struct page *page) 
{
    if (page == NULL) {
        return;
    }
    struct Node *new_node = malloc(sizeof(struct Node));
    if (new_node == NULL) {
        return;
    }
    new_node->p = malloc(sizeof(struct page));
    if (new_node->p == NULL) {
        return;
    }

    new_node->p->id = page->id;
    strcpy(new_node->p->url, page->url);
    new_node->p->description = malloc(strlen(page->description)+1);
    if (new_node->p->description == NULL) {
        return;
    }
    strcpy(new_node->p->description, page->description);
    new_node->next = s->top;
    s->top = new_node;
}

/*ma folosesc de v[i - 1], pentru ca, din cauza conditiei din for, 
i-ul se va mai incrementa inca o data*/

void page(struct browser *b, struct page *v, int id, FILE *out, int nrP) 
{
    int i, ok = 0;

    for (i = 0; i < nrP && ok == 0; i++) {
        if (v[i].id == id) {
            ok = 1; 
        }
    }
    if (ok == 0) {
        fprintf(out, "403 Forbidden\n");
        return;
    }

    add_to_stack(b->current->backwardStack, b->current->currentPage);
    b->current->currentPage->id = v[i - 1].id;
    strcpy(b->current->currentPage->url, v[i - 1].url);
    free(b->current->currentPage->description);
    b->current->currentPage->description = malloc(strlen(v[i - 1].description) + 1);
    strcpy(b->current->currentPage->description, v[i - 1].description);

    free_stack(b->current->forwardStack);
}

//cu reversed_forward imi salvez forwardStack inversat, pentru a afisa corect

void print_history(struct browser *b, int id, FILE *f) 
{
    struct tabsList *p = b->list->next;
    struct Node *current;
    struct stack *reversed_forward = malloc(sizeof(struct stack));
    if (reversed_forward == NULL) {
        return;
    }
    reversed_forward->top = NULL;

    while (p != b->list && p->t->id != id) {
        p = p->next;
    }
    if (p == b->list) {
        fprintf(f, "403 Forbidden\n");
        free_stack_completely(reversed_forward);
        return;
    }
    current = p->t->forwardStack->top;
    while (current != NULL) {
        add_to_stack(reversed_forward, current->p);
        current = current->next;
    }
    if (reversed_forward != NULL) {
        current = reversed_forward->top;
        while (current != NULL) {
            fprintf(f, "%s\n", current->p->url);
            current = current->next;
        }
    }
    if (p->t->currentPage != NULL) {
        fprintf(f, "%s\n", p->t->currentPage->url);
    }
    if (p->t->backwardStack != NULL) {
        current = p->t->backwardStack->top;
        while (current != NULL) {
            fprintf(f, "%s\n", current->p->url);
            current = current->next;
        }
    }

    free_stack_completely(reversed_forward);
}

/*elimin pagina din varful stivei, eliberand memoria
dedicata pentru aceasta si o returnez*/

struct page *extract_page(struct stack *s) 
{
    if (s == NULL || s->top == NULL) {
        return NULL;
    }

    struct Node *tmp = s->top;
    struct page *page = s->top->p, *copy;
    copy = malloc(sizeof(struct page));
    if (copy == NULL) {
        return NULL;
    }

    copy->id = page->id;
    strcpy(copy->url, page->url);
    copy->description = malloc(strlen(page->description) + 1);
    if (copy->description == NULL) {
        return NULL;
    }
    strcpy(copy->description, page->description);
    s->top = s->top->next;

    free(tmp->p->description);
    free(tmp->p);
    free(tmp);  

    return copy;
}

/*copy imi va da ultima pagina adaugata in stiva backwardStack, dupa care
copiez toate elementele ei in pagina tab-ului curent*/

void backward(struct browser *b, FILE *f) 
{
    struct tab *t = b->current;
    struct page *copy;

    if (t->backwardStack == NULL || t->backwardStack->top == NULL) {
        fprintf(f, "403 Forbidden\n");
        return;
    }
    if (t->currentPage != NULL) {
        add_to_stack(t->forwardStack, t->currentPage);
    }
    copy = extract_page(t->backwardStack);
    
    t->currentPage->id = copy->id;
    strcpy(t->currentPage->url, copy->url);
    free(t->currentPage->description);
    t->currentPage->description = malloc(strlen(copy->description) + 1);
    if (t->currentPage->description == NULL) {
        return;
    }
    strcpy(t->currentPage->description, copy->description);

    free(copy->description);
    free(copy);
}

/*ma folosesc de copy pentru a accesa ultima pagina in forwardStack
si o setez ca pagina curenta in tabul curent */

void forward(struct browser *b, FILE *f) 
{
    struct tab *t = b->current;
    struct page *copy;

    if (t->forwardStack == NULL || t->forwardStack->top == NULL) { 
        fprintf(f, "403 Forbidden\n");
        return;
    }
    if (t->currentPage != NULL) {
        add_to_stack(t->backwardStack, t->currentPage);
    }
    copy = extract_page(t->forwardStack);
    
    t->currentPage->id = copy->id;
    strcpy(t->currentPage->url, copy->url);
    free(t->currentPage->description);
    t->currentPage->description = malloc(strlen(copy->description) + 1);
    if (t->currentPage->description == NULL) {
        return;
    }
    strcpy(t->currentPage->description, copy->description);

    free(copy->description);
    free(copy);
}

/*free_pages o folosesc pentru dealocarea memoriei
pentru vectorul de pagini*/

void free_pages(struct page *v, int nrP) 
{
    if (v == NULL) {
        return;
    }

    int i;
    for (i = 0; i < nrP; i++) {
        free(v[i].description);
    }

    free(v);
}

//eliberez memoria pentru tot browser-ul

void free_all(struct browser *b) 
{
    if (b == NULL) {
        return;
    }

    struct tabsList *p = b->list->next, *tmp = NULL;
    while (p != b->list) {
        tmp = p;
        p = p->next;
        free(tmp->t->currentPage->description);
        free(tmp->t->currentPage);
        free_stack_completely(tmp->t->forwardStack);
        free_stack_completely(tmp->t->backwardStack);
        free(tmp->t);
        free(tmp);
    }

    free(b->list);
    free(b);
}

/*convertesc char la int, folosindu-ma de line[i] - '0',
care imi da valorea exacta a caracterului char*/

int conversion(char *line) 
{
    int number, i, length;
    number = i = 0;
    length = strlen(line) - 1;

    for (i = 0; i < length; i++) {
        number = number * 10 + (line[i] - '0');
    }

    return number;
}

int main() 
{
    FILE *f = fopen("tema1.in", "r");
    FILE *out = fopen("tema1.out", "w");
    struct browser *b;
    struct page *v;
    int count_pages, i, count_commands, id, last_tab_id = 0;
    char line[256], id2[256];

    if (f == NULL || out == NULL) {
        printf("error");
        exit(1);
    }
    b = create_browser();
    fgets(line, sizeof(line), f);
    count_pages = conversion(line);

    v = malloc(count_pages * sizeof(struct page));
    if (v == NULL) {
        printf("error");
        return 1;
    }
    for (i = 0; i < count_pages; i++) {
        fgets(line, sizeof(line), f);
        if (line[strlen(line) - 1] == '\n') {
            line[strlen(line) - 1] = '\0';
        }
        v[i].id = atoi(line);
        fgets(line, sizeof(line), f);
        line[strlen(line) - 1] = '\0';
        strcpy(v[i].url, line);
        fgets(line, sizeof(line), f);
        v[i].description = malloc(strlen(line) + 1);
        if (v[i].description == NULL) {
            return 1;
        }
        strcpy(v[i].description, line);
    }

    fgets(line, sizeof(line), f);
    count_commands = conversion(line);
    for (i = 0; i < count_commands; i++) {
        fgets(line, sizeof(line), f);
        if (line[strlen(line) - 1] == '\n') {
            line[strlen(line) - 1] = '\0';
        }
        if (strstr(line, "NEW_TAB") != NULL) {
            new_tab(b, &last_tab_id); 
        } else if (strstr(line, "CLOSE") != NULL) {
            close(b, out);
        } else if (strstr(line, "OPEN") != NULL) {
            strcpy(id2, line + 5);
            id = atoi(id2);
            open(b, id, out);
        } else if (strstr(line, "NEXT") != NULL) {
            next(b);
        } else if (strstr(line, "PREV") != NULL) {
            prev(b);
        } else if (strstr(line, "PAGE") != NULL) {
            strcpy(id2, line + 5);
            id = atoi(id2);
            page(b, v, id, out, count_pages);
        } else if (strstr(line, "PRINT_HISTORY") != NULL) {
            strcpy(id2, line + 14);
            id = atoi(id2);
            print_history(b, id, out);
        } else if (strstr(line, "PRINT") != NULL) {
            print(b, out);
        } else if (strstr(line, "BACKWARD") != NULL) {
            backward(b, out);
        } else if (strstr(line, "FORWARD") != NULL) {
            forward(b, out);
        }
    }

    free_all(b);
    free_pages(v, count_pages);
    fclose(f);
    fclose(out);
}