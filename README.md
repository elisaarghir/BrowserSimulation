# BrowserSimulation
The project represents a simulation of a web browser history management architecture, modeled after mechanisms employed in modern browsers, utilizing advanced data structures


# Summary of Implementation
Data Structures:
 - Implemented the stack as a singly linked list, each node containing a page and a pointer to the next node.
 - The stack structure maintains a pointer to the top element.
 - Browser tabs are managed in a circular doubly linked list with a sentinel node.

# Core Functions:

 - tabs(): allocates memory for a new tab, sets the default page, and initializes its stacks.
 - create_browser(): initializes the sentinel and adds the first tab.
 - new_tab(): creates and links a new tab, updating IDs and circular references.
 - print(): traverses the list to find the current tab, then displays its content while skipping the sentinel.
 - free_stack() / free_stack_completely(): free stack elements, with the latter also releasing the stack itself.
 - close(): removes the current tab, updates links, and frees associated memory.
 - open(): finds and sets the current tab by ID.
 - next() / prev(): switch between adjacent tabs, handling the circular list.
 - add_to_stack(): pushes a new page onto a stack.
 - page(): loads a page by ID, updates current/previous pages, and resets the forward stack.
 - print_history(): reconstructs and prints browsing history using an auxiliary reversed stack.
 - extract_page(): safely copies and pops a page from a stack.
 - backward() / forward(): navigate history by moving pages between stacks and updating the current page.
 - free_pages() / free_all(): properly release memory for pages, tabs, and all dynamically allocated fields.

# Notes:

 - Special care was taken to handle the sentinel node in the circular list.
 -  Memory management was thoroughly implemented to avoid leaks, using Valgrind for debugging.
