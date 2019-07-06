typedef struct {
    char plate[8];
    char cam[5];
    int speed;
    char unit[5];
} cam_data;

typedef struct {
    cam_data cam_data;
    int amount;
    char hora[6];
    char date[11];
} ticket_data;