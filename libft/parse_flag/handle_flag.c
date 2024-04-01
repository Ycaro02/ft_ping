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

int8_t search_exist_opt(t_list *opt_lst, uint8_t c)
{
    for (t_list *tmp = opt_lst; tmp; tmp = tmp->next) {
        if (((t_opt_node *)tmp->content)->flag == c) {
            return (1);
        }
    }
    return (0);
}

int8_t add_flag_option(t_flag_context *flag_c, int8_t c, uint32_t value)
{
    t_opt_node *opt = NULL;

    if (!flag_c) {
        ft_printf_fd(2, "Invalid list option addr\n");
        return (0);
    } else if (search_exist_opt(flag_c->opt_lst, c)) {
        ft_printf_fd(2, "Opt %c already present\n", c);
        return (0);
    }
    opt = ft_calloc(sizeof(t_opt_node), 1);
    if (!opt) {
        ft_printf_fd(2, "Failed to allocate memory for opt\n");
        return (0);
    }
    ft_lstadd_back(&flag_c->opt_lst, ft_lstnew(opt));
    
    char *new_char = ft_strjoin("", c);
    char to_free = flag_c->opt_str == NULL ? 's' : 'a';
    flag_c->opt_str = ft_strjoin_free(flag_c->opt_str, new_char, to_free);
    return (1);
}
