#if !defined(S_USER_H)
#define S_USER_H

/*=============================================================================
 * Macro's
 */

/*
 * Nick flood limit
 * Minimum time between nick changes.
 * (The first two changes are allowed quickly after another however).
 */
#define NICK_DELAY 30

/*
 * Target flood time.
 * Minimum time between target changes.
 * (MAXTARGETS are allowed simultaneously however).
 * Its set to a power of 2 because we devide through it quite a lot.
 */
#define TARGET_DELAY 128

/* return values for hunt_server() */

#define HUNTED_NOSUCH	(-1)      /* if the hunted server is not found */
#define HUNTED_ISME	0           /* if this server should execute the command */
#define HUNTED_PASS	1           /* if message passed onwards successfully */

/* used when sending to #mask or $mask */

#define MATCH_SERVER  1
#define MATCH_HOST    2

/*=============================================================================
 * Proto types
 */

extern int m_umode(aClient *cptr, aClient *sptr, int parc, char *parv[]);
extern int m_svsumode(aClient *cptr, aClient *sptr, int parc, char *parv[]);
extern int is_silenced(aClient *sptr, aClient *acptr);
extern int hunt_server(int, aClient *cptr, aClient *sptr,
    char *command, char *token, const char *pattern, int server, int parc, char *parv[]);
extern aClient *next_client(aClient *next, char *ch);
extern int m_nick(aClient *cptr, aClient *sptr, int parc, char *parv[]);
extern int m_private(aClient *cptr, aClient *sptr, int parc, char *parv[]);
extern int m_notice(aClient *cptr, aClient *sptr, int parc, char *parv[]);
extern int m_wallchops(aClient *cptr, aClient *sptr, int parc, char *parv[]);
extern int m_cprivmsg(aClient *cptr, aClient *sptr, int parc, char *parv[]);
extern int m_cnotice(aClient *cptr, aClient *sptr, int parc, char *parv[]);
extern int m_user(aClient *cptr, aClient *sptr, int parc, char *parv[]);
extern int m_webirc(aClient *cptr, aClient *sptr, int parc, char *parv[]);
extern int m_proxy(aClient *cptr, aClient *sptr, int parc, char *parv[]);
extern int m_quit(aClient *cptr, aClient *sptr, int parc, char *parv[]);
extern int m_kill(aClient *cptr, aClient *sptr, int parc, char *parv[]);
extern int m_away(aClient *cptr, aClient *sptr, int parc, char *parv[]);
extern int m_ping(aClient *cptr, aClient *sptr, int parc, char *parv[]);
extern int m_pong(aClient *cptr, aClient *sptr, int parc, char *parv[]);
extern int m_oper(aClient *cptr, aClient *sptr, int parc, char *parv[]);
extern int m_pass(aClient *cptr, aClient *sptr, int parc, char *parv[]);
extern int m_userhost(aClient *cptr, aClient *sptr, int parc, char *parv[]);
extern int m_userip(aClient *cptr, aClient *sptr, int parc, char *parv[]);
extern int m_ison(aClient *cptr, aClient *sptr, int parc, char *parv[]);
extern int m_privs(aClient *cptr, aClient *sptr, int parc, char *parv[]);
extern void send_features(aClient *sptr, char *nick);
extern char *umode_str(aClient *cptr, aClient *acptr);
extern void send_umode(aClient *cptr, aClient *sptr, int old, int sendmask,
    int oldh, int sendhmask);
extern int del_silence(aClient *sptr, char *mask);
extern int m_silence(aClient *cptr, aClient *sptr, int parc, char *parv[]);
extern void set_snomask(aClient *, snomask_t, int);
extern int is_snomask(char *);
extern int check_target_limit(aClient *sptr, void *target, const char *name,
    int created);
extern void add_target(aClient *sptr, void *target);
extern int m_ghost(aClient *cptr, aClient *sptr, int parc, char *parv[]);
extern int get_status(aClient *sptr);
#if defined(BDD_VIP)
extern int can_viewhost(aClient *sptr, aClient *acptr, int audit);
extern char *get_virtualhost(aClient *sptr, int perso);
extern char *get_visiblehost(aClient *sptr, aClient *acptr, int audit);
void make_vhost(aClient *acptr, int mostrar);
void make_vhostperso(aClient *acptr, int mostrar);
#endif
extern void set_privs(aClient *sptr);
extern uint64_t HasPriv(aClient *sptr, uint64_t priv);
extern void rename_user(aClient *sptr, char *nick_nuevo);
extern void mask_user_flags(char *modes, int *addflags, int *addhmodes);
extern void send_umode_out(aClient *cptr, aClient *sptr, int old, int oldh, int registrado);

extern int m_svsnick(aClient *cptr, aClient *sptr, int parc, char *parv[]);

extern struct SLink *opsarray[];

#endif /* S_USER_H */
