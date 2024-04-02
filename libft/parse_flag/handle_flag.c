#include "parse_flag.h"

/**
* 00000000 -> Our current flag value is 0
* | 00000100 -> Do or operation with Flag3, which has a value of 4
* = 00000100 -> The bit for Flag3 gets set to 1, flag value is now 4
* Enable target flag
*/
void set_flag(int *flags, int flag_val)
{
    if (flags)
        *flags |= flag_val;
}

/**
* 00000100 -> We get Flag3 as input to unset
* ~= 11111011 -> We flip all the bits of Flag3
* 00010110 -> Our current value had Flag2, 3 and 5 set already
* &  11111011 -> AND operation with the result of previous inversion
* =  00010010 -> Our new value only has Flag2 and 5 set
* Disable target flag
*/
void unset_flag(int *flags, int flag_val)
{
    if (flags)
        *flags &= ~flag_val;
}

/*
* 00010110 -> Starting value has Flag2, Flag3 and Flag5 set
* & 00000100 -> Perform & with Flag3
* = 00000100 -> Result is equal to Flag3
* check if flag_val enable in flags
// __always_inline int has_flag(int flags, int flag_val)
*/

int8_t has_flag(int flags, int flag_val) {
   return ((flags & flag_val) == flag_val);
}

int8_t flag_already_present(int flags, int flag_val) {
    return (has_flag(flags, flag_val));
}

/* Start option handle */

void display_option_list(t_flag_context *flag_c, t_list *opt_lst)
{
    t_opt_node *node = NULL; 
    for (t_list *tmp = opt_lst; tmp; tmp = tmp->next) {
        node = tmp->content;
        ft_printf_fd(1, "Flag: %c, Flag_val %d Value: %d str : %s\n", node->flag_char, node->flag_val, node->value, flag_c->opt_str);
    }
}


static int8_t is_same_char_opt(void *content, uint32_t c) {
    return ((uint8_t)((t_opt_node *)content)->flag_char == c);
}

static int8_t is_same_flag_val_opt(void *content, uint32_t value) {
    return (((t_opt_node *)content)->flag_val == value);
}

static int8_t search_exist_opt(t_list *opt_lst, int8_t (cmp(void *, uint32_t)), uint32_t test)
{
    for (t_list *tmp = opt_lst; tmp; tmp = tmp->next) {
        if (cmp(tmp->content, test)) {
            return (1);
        }
    }
    return (0);
}

static t_opt_node *create_opt_node(uint8_t c, uint32_t flag_val, uint32_t value)
{
    t_opt_node *opt = ft_calloc(sizeof(t_opt_node), 1);

    if (!opt) {
        ft_printf_fd(2, "Failed to allocate memory for opt\n");
        return (NULL);
    }
    opt->flag_char = c;
    opt->flag_val = flag_val;
    opt->value = value;
    return (opt);
}


static int8_t update_opt_str(t_flag_context *flag_c, uint8_t c)
{
    char new_char[2] = {c, 0};
    if (flag_c->opt_str == NULL) {
        flag_c->opt_str = ft_strdup(new_char);
    } else {
        flag_c->opt_str = ft_strjoin_free(flag_c->opt_str, new_char, 'f');
    }
    if (!flag_c->opt_str) {
        ft_printf_fd(2, "Failed to allocate memory for opt_str\n");
        return (0);
    }
    return (1);
}

int8_t add_flag_option(t_flag_context *flag_c, uint8_t c, uint32_t flag_val, uint32_t value)
{
    t_opt_node *opt = NULL;
    int8_t ret = 0;

    if (!flag_c) {
        ft_printf_fd(2, "Invalid list option addr\n");
        return (0);
    } 
    else if (search_exist_opt(flag_c->opt_lst, is_same_char_opt, c)\
        || search_exist_opt(flag_c->opt_lst, is_same_flag_val_opt, flag_val)) {
        ft_printf_fd(2, RED"Opt char |%c| or flag val |%d| already present\n"RESET, c, flag_val);
        return (0);
    }
    opt = create_opt_node(c, flag_val, value);
    ft_lstadd_back(&flag_c->opt_lst, ft_lstnew(opt));
    ret = update_opt_str(flag_c, c); 
    return (ret);
}

void free_flag_context(t_flag_context *flag_c)
{
    if (flag_c) {
        if (flag_c->opt_str) {
            free(flag_c->opt_str);
        }
        if (flag_c->opt_lst) {
            ft_lstclear(&flag_c->opt_lst, free);
        }
        // free(flag_c);
    }
}

void test_opt()
{
	t_flag_context flag_c;

	ft_bzero(&flag_c, sizeof(t_flag_context));
	ft_printf_fd(1, "Test opt\n");
	add_flag_option(&flag_c, 'v', 1, -1);
	add_flag_option(&flag_c, 'c', 2, -1);
	add_flag_option(&flag_c, 'v', 1, -1);
	add_flag_option(&flag_c, '2', 1, -1);
	display_option_list(&flag_c, flag_c.opt_lst);
	free_flag_context(&flag_c);
}
