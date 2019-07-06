#define SPEED_LIMIT 60
#define AMOUNT 1500

#define READ_ONLY "r"

#define DUMP_FILE 1
#define TICKET_FILE 2

#define TICKET(X) (X > SPEED_LIMIT) ? 1 : 0
#define TICKET_AMOUNT(X) (X - SPEED_LIMIT) * AMOUNT