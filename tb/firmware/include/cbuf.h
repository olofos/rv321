// Inspired by https://github.com/dhylands/TimerUART/blob/master/CBUF.h

#ifndef CBUF_H_
#define CBUF_H_

/*
Basic usage:

// Length must be a power of two and strictly less than 2^sizeof(head),
// otherwise we can't distinguish between an empty buffer and a full buffer

#define my_cbuf_LEN 16;
struct
{
    uint8_t head;
    uint8_t tail;
    uint8_t buf[my_cbuf_LEN];
} my_cbuf;

cbuf_init(my_cbuf);
cbuf_push(my_cbuf, 'A');
c = cbuf_pop(my_cbuf);

*/

/*
For storing larger structs we can also work directly on the inline storage:

#define cbuf_LEN 16;
struct data { int a; int b; };
struct
{
    uint8_t head;
    uint8_t tail;
    struct data buf[cbuf_LEN];
} cbuf;

cbuf_init(cbuf);

cbuf_head(cbuf)->a = 1;
cbuf_head(cbuf)->b = 2;
cbuf_head_next(cbuf);

function_using_data(cbuf_tail(cbuf));
cbuf_tail_next(cbuf);

*/

// Would be even better to check that head and tail have the same type, and if they are unsigned
// This should be possible with some _Generic macro magic, but let's trust the user

#define cbuf_init(cbuf) do {                                            \
        _Static_assert(((cbuf##_LEN)&((cbuf##_LEN)-1)) == 0, "Length of " #cbuf " must be a power of two"); \
        _Static_assert((cbuf##_LEN) < (1 << (8 * sizeof(cbuf.head))), "Length of " #cbuf " must be less than range of head"); \
        _Static_assert(sizeof(cbuf.head) == sizeof(cbuf.tail), #cbuf " head and tail should have the same size"); \
        cbuf.head = cbuf.tail = 0;                                      \
    } while(0)

// Number of elements currently in the buffer
#define cbuf_len(cbuf) ( (typeof(cbuf.head)) ((cbuf.head) - (cbuf.tail)) )

#define cbuf_push(cbuf, elem)  do { (cbuf.buf)[ (cbuf.head) & ((cbuf##_LEN) - 1) ] = (elem); cbuf.head++; } while(0);

#define cbuf_pop(cbuf) (cbuf.buf)[ cbuf.tail++ & ((cbuf##_LEN) - 1) ]

#define cbuf_peek(cbuf) (cbuf.buf)[ cbuf.tail & ((cbuf##_LEN) - 1) ]

#define cbuf_full(cbuf) ( cbuf_len(cbuf) == (cbuf##_LEN) )

#define cbuf_empty(cbuf) ( cbuf_len(cbuf) == 0 )

#define cbuf_head(cbuf) (&(cbuf.buf[ cbuf.head & ((cbuf##_LEN) - 1) ]))
#define cbuf_tail(cbuf) (&(cbuf.buf[ cbuf.tail & ((cbuf##_LEN) - 1) ]))

#define cbuf_head_next(cbuf) do { cbuf.head++; } while(0)
#define cbuf_tail_next(cbuf) do { cbuf.tail++; } while(0)

#define cbuf_linear_len(cbuf) ((cbuf_head(cbuf) > cbuf_tail(cbuf)) ? cbuf_len(cbuf) : (cbuf_end(cbuf) - cbuf_tail(cbuf) + 1))

#define cbuf_capacity(cbuf) ((typeof(cbuf.head)) ((cbuf##_LEN) - cbuf_len(cbuf)))

#define cbuf_start(cbuf) (&(cbuf.buf[0]))
#define cbuf_end(cbuf) (&(cbuf.buf[(cbuf##_LEN) - 1]))


#endif
