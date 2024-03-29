#ifndef STATUSCODE_H
#define STATUSCODE_H

#define STATUS_CODE_REGISTER_NORMAL                 (100)
#define STATUS_CODE_REGISTER                        (101)
#define STATUS_CODE_LOGIN                           (102)
#define STATUS_CODE_LOGOUT                          (103)
#define STATUS_CODE_MSG_SEND_GROUP                  (104)
#define STATUS_CODE_MSG_SEND_PRIVATE                (105)

#define STATUS_CODE_SUCESSFUL                       (200)
#define STATUS_CODE_REGISTER_SUCESSFUL              (201)
#define STATUS_CODE_LOGIN_SUCESSFUL                 (202)
#define STATUS_CODE_MSG_BOARDCAST                   (203)
#define STATUS_CODE_MSG_PRIVATE                     (204)

#define STATUS_CODE_ERROR                           (400)
#define STATUS_CODE_REGISTER_FAILED_TIMEOUT         (401)
#define STATUS_CODE_REGISTER_FAILED_USERNAME_EXSIST (402)
#define STATUS_CODE_LOGIN_FAILED_WROWN_PASSWORD     (403)
#define STATUS_CODE_LOGIN_FAILED_USER_NOT_FOUND     (404)

#endif // STATUSCODE_H
