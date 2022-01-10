  █████   ▒█████   ██▀███    ▄████ 
▒██▓  ██▒▒██▒  ██▒▓██ ▒ ██▒ ██▒ ▀█▒
▒██▒  ██░▒██░  ██▒▓██ ░▄█ ▒▒██░▄▄▄░
░██  █▀ ░▒██   ██░▒██▀▀█▄  ░▓█  ██▓
░▒███▒█▄ ░ ████▓▒░░██▓ ▒██▒░▒▓███▀▒
░░ ▒▒░ ▒ ░ ▒░▒░▒░ ░ ▒▓ ░▒▓░ ░▒   ▒ 
 ░ ▒░  ░   ░ ▒ ▒░   ░▒ ░ ▒░  ░   ░ 
   ░   ░ ░ ░ ░ ▒    ░░   ░ ░ ░   ░ 
    ░        ░ ░     ░           ░ 
                                   


/====================================================================================
|fingerd, a finger daemon                                                           |
|                                                                                   |
| USAGE                                                                             |
|    If no arguments are given, binds to 0.0.0.0 port 79. It returns                |
|    The ~/.plan file to the client.                                                |
|                                                                                   |
| WHY?                                                                              |
|    Boredom.                                                                       |
| IS THIS SECURE?                                                                   |
|    No idea, i used snprintf() and strncpy() so there are not buffer overflows     |
|    But don't trust this thing a lot.                                              |
| TODO                                                                              |
     Maybe use asprintf() instead of snprintf() and strncpy()                       |
| BUGS                                                                              |
|                                                                                   |
|    Sure. e-mail me them, or xmpp, whatever.                                       |
|                                                                                   |
\====================================================================================

