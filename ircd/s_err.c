/*
 * IRC - Internet Relay Chat, ircd/s_err.c
 * Copyright (C) 1992 Darren Reed
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 1, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "sys.h"
#include "h.h"
#include "s_debug.h"
#include "numeric.h"
#include "s_err.h"
#include "sprintf_irc.h"
#include "m_watch.h"

typedef struct {
  int num_val;
  char *num_form;
} Numeric;

/* *INDENT-OFF* */

static Numeric local_replies[] = {
/* 000 */
    {0, (char *)NULL},
/* 001 */
    {RPL_WELCOME, ":Welcome to the %s IRC Network, %s"},
/* 002 */
    {RPL_YOURHOST, ":Your host is %s, running version %s"},
/* 003 */
    {RPL_CREATED, ":This server was created %s"},

/* 004 */
#if defined(BDD_VIP)
    {RPL_MYINFO, "%s %s dioswkgxXrRaChSBcnz biklmnopstvcrRMCNuWz"},
#else
    {RPL_MYINFO, "%s %s dioswkgraChSBcnz biklmnopstvcrRMCNuWz"},
#endif /* BDD_VIP */

/* 005 */
    {RPL_ISUPPORT, "%s :are supported by this server"},
/* 006 */
    {0, (char *)NULL},
/* 007 */
    {0, (char *)NULL},
/* 008 */
    {RPL_SNOMASK, "%d :: Server notice mask (%#x)"},
/* 009 */
    {RPL_STATMEMTOT, "%u %u :Bytes Blocks"},
/* 010 */
#if defined(MEMSIZESTATS)
    {RPL_STATMEM, "%u %u %s %u"},
#else
    {RPL_STATMEM, "%u %u %s"},
#endif
/* 011 */
    {0, (char *)NULL},
/* 012 */
    {0, (char *)NULL},
/* 013 */
    {0, (char *)NULL},
/* 014 */
    {0, (char *)NULL},
/* 015 */
    {RPL_MAP, ":%s%s%s (%s) %s [%i clientes - %i.%i%%]"},
/* 016 */
    {RPL_MAPMORE, ":%s%s --> *more*"},
/* 017 */
    {RPL_MAPEND, ":End of /MAP"},
    
    {0, (char *)NULL}
};

static Numeric numeric_errors[] = {
/* 401 */
    {ERR_NOSUCHNICK, "%s :No such nick - Nick no presente en IRC"},
/* 402 */
    {ERR_NOSUCHSERVER, "%s :No such server"},
/* 403 */
    {ERR_NOSUCHCHANNEL, "%s :No such channel - Canal inexistente"},
/* 404 */
    {ERR_CANNOTSENDTOCHAN, "%s :Cannot send to channel - "
                           "No se puede enviar texto al canal"},
/* 405 */
    {ERR_TOOMANYCHANNELS, "%s :You have joined too many channels - "
                          "Ha llegado al m�ximo de canales permitidos en este servidor"},
/* 406 */
    {ERR_WASNOSUCHNICK, "%s :There was no such nickname - "
                        "El nick que busca no estaba en IRC"},
/* 407 */
    {ERR_TOOMANYTARGETS, "%s :Duplicate recipients. No message delivered"},
/* 408 */
    {0, (char *)NULL},
/* 409 */
    {ERR_NOORIGIN, ":No origin specified"},
/* 410 */
    {0, (char *)NULL},
/* 411 */
    {ERR_NORECIPIENT, ":No recipient given (%s)"},
/* 412 */
    {ERR_NOTEXTTOSEND, ":No text to send"},
/* 413 */
    {ERR_NOTOPLEVEL, "%s :No toplevel domain specified"},
/* 414 */
    {ERR_WILDTOPLEVEL, "%s :Wildcard in toplevel Domain"},
/* 415 */
    {0, (char *)NULL},
/* 416 */
    {ERR_QUERYTOOLONG, "%s :Too many lines in the output, restrict your query"},
/* 417 */
    {ERR_INPUTTOOLONG, ":Input line was too long"},
/* 418 */
    {0, (char *)NULL},
/* 419 */
    {0, (char *)NULL},
/* 420 */
    {0, (char *)NULL},
/* 421 */
    {ERR_UNKNOWNCOMMAND, "%s :Unknown command - Comando desconocido"},
/* 422 */
    {ERR_NOMOTD, ":MOTD File is missing"},
/* 423 */
    {ERR_NOADMININFO, "%s :No administrative info available"},
/* 424 */
    {0, (char *)NULL},
/* 425 */
    {0, (char *)NULL},
/* 426 */
    {0, (char *)NULL},
/* 427 */
    {0, (char *)NULL},
/* 428 */
    {0, (char *)NULL},
/* 429 */
    {0, (char *)NULL},
/* 430 */
    {0, (char *)NULL},
/* 431 */
    {ERR_NONICKNAMEGIVEN, ":No nickname given"},
/* 432 */
    {ERR_INVALIDNICKNAME, "%s :Invalid Nickname"},
/* 433 */
    {ERR_NICKNAMEINUSE, "%s :Nickname is already in use - "
                        "El nick que intenta usar ya se encuentra en IRC "},
/* 434 */
    {0, (char *)NULL},
/* 435 */
    {0, (char *)NULL},
/* 436 */
    {ERR_NICKCOLLISION, "%s :Nickname collision KILL"},
/* 437 */
    {ERR_BANNICKCHANGE, "%s :Cannot change nickname while banned on channel"},
/* 438 */
    {ERR_NICKTOOFAST, "%1$s :Nick change too fast. Please wait %2$d seconds - "
                      "Necesita esperar %2$d segundos para volver a "
                      "cambiarse el nick"},
/* 439 */
    {ERR_TARGETTOOFAST, "%1$s :Target change too fast. Please wait %2$d seconds - "
                        "Debe esperar %2$d segundos para entrar en otro canal "
                        "o abrir otro privado"},
/* 440 */
    {0, (char *)NULL},
/* 441 */
    {ERR_USERNOTINCHANNEL, "%s %s :They aren't on that channel"},
/* 442 */
    {ERR_NOTONCHANNEL, "%s :You're not on that channel"},
/* 443 */
    {ERR_USERONCHANNEL, "%s %s :is already on channel"},
/* 444 */
    {0, (char *)NULL},
/* 445 */
    {0, (char *)NULL},
/* 446 */
    {0, (char *)NULL},
/* 447 */
    {0, (char *)NULL},
/* 448 */
    {0, (char *)NULL},
/* 449 */
    {0, (char *)NULL},
/* 450 */
    {0, (char *)NULL},
/* 451 */
    {ERR_NOTREGISTERED, ":You have not registered"},
/* 452 */
    {0, (char *)NULL},
/* 453 */
    {0, (char *)NULL},
/* 454 */
    {0, (char *)NULL},
/* 455 */
    {0, (char *)NULL},
/* 456 */
    {0, (char *)NULL},
/* 457 */
    {0, (char *)NULL},
/* 458 */
    {0, (char *)NULL},
/* 459 */
    {0, (char *)NULL},
/* 460 */
    {0, (char *)NULL},
/* 461 */
    {ERR_NEEDMOREPARAMS, "%s :Not enough parameters"},
/* 462 */
    {ERR_ALREADYREGISTRED, ":You may not reregister"},
/* 463 */
    {ERR_NOPERMFORHOST, ":Your host isn't among the privileged"},
/* 464 */
    {ERR_PASSWDMISMATCH, ":Password Incorrect"},
/* 465 */
    {ERR_YOUREBANNEDCREEP, ":You are banned from this server"},
/* 466 */
    {ERR_YOUWILLBEBANNED, (char *)NULL},
/* 467 */
    {ERR_KEYSET, "%s :Channel key already set"},
/* 468 */
    {ERR_INVALIDUSERNAME, (char *)NULL},
/* 469 */
    {0, (char *)NULL},
/* 470 */
    {ERR_LINKCHANNEL, "%s %s :Forwarding to another channel"},
/* 471 */
    {ERR_CHANNELISFULL, "%s :Cannot join channel (+l) - "
                        "El canal est� limitado y no acepta m�s usuarios"},
/* 472 */
    {ERR_UNKNOWNMODE, "%c :is unknown mode char to me"},
/* 473 */
    {ERR_INVITEONLYCHAN, "%s :Cannot join channel (+i) - "
                         "Necesita ser invitado para poder entrar en el canal"},
/* 474 */
    {ERR_BANNEDFROMCHAN, "%s :Cannot join channel (+b) - "
                         "No puedes entrar, estas baneado en el canal"},
/* 475 */
    {ERR_BADCHANNELKEY, "%s :Cannot join channel (+k) - "
                        "Necesita contrase�a para entrar en el canal"},
/* 476 */
    {ERR_BADCHANMASK, "%s :Bad Channel Mask"},
/* 477 */
    {ERR_NEEDREGGEDNICK, "%s :Necesita registrar su nick para poder entrar en el canal, "
	                 "para m�s informaci�n escriba: /msg NiCK help REGISTER"},
/* 478 */
    {ERR_BANLISTFULL, "%s %s :Channel ban/ignore list is full"},
/* 479 */
    {ERR_BADCHANNAME, "%s :Cannot join channel (access denied on this server)"},
/* 480 */
    {0, (char *)NULL},
/* 481 */
    {ERR_NOPRIVILEGES, ":No tienes privilegios para ejecutar el comando"},
/* 482 */
    {ERR_CHANOPRIVSNEEDED, "%s :You're not channel operator - "
                           "Necesita ser operador (@) del canal para realizar esa acci�n"},
/* 483 */
    {ERR_CANTKILLSERVER, ":You cant kill a server!"},
/* 484 */
    {ERR_ISCHANSERVICE, "%s %s :Cannot kick or deop an IRC Operator"},
/* 485 */
    {ERR_ISREALSERVICE, "%s %s :Cannot kill, kick or deop a network service"},
/* 486 */
    {ERR_NONONREG, ":Necesita registrar su nick para poder hablar con %s, "
	           "para m�s informaci�n escriba: /msg NiCK help REGISTER"},
/* 487 */
    {ERR_ISOWNERCHAN, "%s %s :Cannot kick an owner channel"},
/* 488 */
    {ERR_NOSSL, "%s :Cannot join channel (+z) - "
                        "Necesita conectar de forma segura con SSL para entrar al canal"},
/* 489 */
    {ERR_VOICENEEDED, "%s :You're neither voiced nor channel operator"},
/* 490 */
    {0, (char *)NULL},
/* 491 */
    {ERR_NOOPERHOST, ":No O-lines for your host"},
/* 492 */
    {ERR_OPERCLASSFULL, ":Class %d is full!"},
/* 493 */
    {ERR_NOOPERCLASS, ":Class %d non-existent"},
/* 494 */
    {0, (char *)NULL},
/* 495 */
    {0, (char *)NULL},
/* 496 */
    {0, (char *)NULL},
/* 497 */
    {0, (char *)NULL},
/* 498 */
    {ERR_ISOPERLCHAN, "%s %s :Cannot kick or deop an IRC Operator on a local channel"},
/* 499 */
    {0, (char *)NULL},
/* 500 */
    {0, (char *)NULL},
/* 501 */
    {ERR_UMODEUNKNOWNFLAG, ":Unknown user MODE flag"},
/* 502 */
    {ERR_USERSDONTMATCH, ":Cant change mode for other users"},
/* 503 */
    {0, (char *)NULL},
/* 504 */
    {0, (char *)NULL},
/* 505 */
    {0, (char *)NULL},
/* 506 */
    {0, (char *)NULL},
/* 507 */
    {0, (char *)NULL},
/* 508 */
    {0, (char *)NULL},
/* 509 */
    {ERR_SILECANTBESHOWN, "%s :You can't see another user silence list - "
                          "No puede ver la lista de \"Silence\" de otros usuarios"},
/* 510 */
    {ERR_ISSILENCING, ":You can't talk to %1$s. (S)he is silencing you - "
                      "El usuario %1$s le ha silenciado, no recibir� sus mensajes"},
/* 511 */
    {ERR_SILELISTFULL, "%s :Your silence list is full - "
                       "Su lista de \"Silence\" est� llena"},
/* 512 */
    {ERR_TOOMANYWATCH, "%s :Maximum size for WATCH-list is %d entries"},
/* 513 */
    {ERR_BADPING, (char *)NULL},
/* 514 */
    {ERR_NOSUCHGLINE, "%s@%s :No such gline"},
/* 515 */
    {0, (char *)NULL},
/* 516 */
    {0, (char *)NULL},
/* 517 */
    {0, (char *)NULL},
/* 518 */
    {0, (char *)NULL},
/* 519 */
    {0, (char *)NULL},
/* 520 */
    {0, (char *)NULL},
/* 521 */
    {0, (char *)NULL},
/* 522 */
    {0, (char *)NULL},
/* 523 */
    {0, (char *)NULL},
/* 524 */
    {0, (char *)NULL},
/* 525 */
    {0, (char *)NULL},
/* 526 */
    {ERR_OPERONLY, "%s :Cannot join channel (+O) - "
                      "Necesita ser IRC Operator para entrar" }
};

static Numeric numeric_replies[] = {
/* 300 */
    {RPL_NONE, (char *)NULL},
/* 301 */
    {RPL_AWAY, "%s :%s"},
/* 302 */
    {RPL_USERHOST, ":"},
/* 303 */
    {RPL_ISON, ":"},
/* 304 */
    {RPL_TEXT, (char *)NULL},
/* 305 */
    {RPL_UNAWAY, ":You are no longer marked as being away - "
                 "Ya no est� ausente, bienvenido/a a casa"},
/* 306 */
    {RPL_NOWAWAY, ":You have been marked as being away - "
                  "Acaba de ponerse ausente, hasta pronto ;-)"},
/* 307 */
    {RPL_WHOISREGNICK, "%s :Tiene el nick Registrado y Protegido"},
/* 308 */
    {0, (char *)NULL},
/* 309 */
    {0, (char *)NULL},
/* 310 */
    {RPL_WHOISHELPOP, "%s :esta disponible para ayuda"},
/* 311 */
    {RPL_WHOISUSER, "%s %s %s * :%s"},
/* 312 */
    {RPL_WHOISSERVER, "%s %s :%s"},
/* 313 */
    {RPL_WHOISOPERATOR, "%s :%s"},
/* 314 */
    {RPL_WHOWASUSER, "%s %s %s * :%s"},
/* 315 */
    {RPL_ENDOFWHO, "%s :End of /WHO list"},
/* 316 */
    {RPL_WHOISBOT, "%s :es un roBOT oficial de la Red"},
/* 317 */
    {RPL_WHOISIDLE, "%s %ld %ld :seconds idle, signon time"},
/* 318 */
    {RPL_ENDOFWHOIS, "%s :End of /WHOIS list"},
/* 319 */
    {RPL_WHOISCHANNELS, "%s :%s"},
/* 320 */
    {0, (char *)NULL},
/* 321 */
    {RPL_LISTSTART, "Channel :Users  Name"},
/* 322 */
    {RPL_LIST, "%s %d :%s"},
/* 323 */
    {RPL_LISTEND, ":End of /LIST"},
/* 324 */
    {RPL_CHANNELMODEIS, "%s %s %s"},
/* 325 */
    {0, (char *)NULL},
/* 326 */
    {0, (char *)NULL},
/* 327 */
    {0, (char *)NULL},
/* 328 */
    {0, (char *)NULL},
/* 329 */
    {RPL_CREATIONTIME, "%s " TIME_T_FMT},
/* 330 */
    {0, (char *)NULL},
/* 331 */
    {RPL_NOTOPIC, "%s :No topic is set"},
/* 332 */
    {RPL_TOPIC, "%s :%s"},
/* 333 */
    {RPL_TOPICWHOTIME, "%s %s " TIME_T_FMT},
/* 334 */
    {RPL_LISTUSAGE, ":%s"},
/* 335 */
    {0, (char *)NULL},
/* 336 */
    {0, (char *)NULL},
/* 337 */
    {RPL_WHOISSSL, "%s :conecta de forma segura con SSL"},
/* 338 */
    {RPL_WHOISACTUALLY, "%s %s@%s %s :Actual user@host, Actual IP"},
/* 339 */
    {0, (char *)NULL},
/* 340 */
    {RPL_USERIP, ":"},
/* 341 */
    {RPL_INVITING, "%s %s"},
/* 342 */
    {RPL_MSGONLYREG, "%s :Solo admite privados de usuarios registrados"},
/* 343 */
    {RPL_DOCKING, "%s :Tiene el servicio de Docking"},
/* 344 */
    {0, (char *)NULL},
/* 345 */
    {0, (char *)NULL},
/* 346 */
    {RPL_INVITELIST, ":%s"},
/* 347 */
    {RPL_ENDOFINVITELIST, ":End of Invite List"},
/* 348 */
    {0, (char *)NULL},
/* 349 */
    {0, (char *)NULL},
/* 350 */
    {0, (char *)NULL},
/* 351 */
    {RPL_VERSION, "%s.%s %s :%s"},
/* 352 */
    {RPL_WHOREPLY, "%s"},
/* 353 */
    {RPL_NAMREPLY, "%s"},
/* 354 */
    {RPL_WHOSPCRPL, "%s"},
/* 355 */
    {0, (char *)NULL},
/* 356 */
    {0, (char *)NULL},
/* 357 */
    {0, (char *)NULL},
/* 358 */
    {0, (char *)NULL},
/* 359 */
    {0, (char *)NULL},
/* 360 */
    {0, (char *)NULL},
/* 361 */
    {RPL_KILLDONE, (char *)NULL},
/* 362 */
    {RPL_CLOSING, "%s :Closed. Status = %d"},
/* 363 */
    {RPL_CLOSEEND, "%d: Connections Closed"},
/* 364 */
#if !defined(GODMODE)
    {RPL_LINKS, "%s %s :%d P%u %s"},
#else /* GODMODE */
    {RPL_LINKS, "%s %s :%d P%u " TIME_T_FMT " (%s) %s"},
#endif /* GODMODE */
/* 365 */
    {RPL_ENDOFLINKS, "%s :End of /LINKS list"},
/* 366 */
    {RPL_ENDOFNAMES, "%s :End of /NAMES list"},
/* 367 */
    {RPL_BANLIST, "%s %s %s " TIME_T_FMT},
/* 368 */
    {RPL_ENDOFBANLIST, "%s :End of Channel Ban List"},
/* 369 */
    {RPL_ENDOFWHOWAS, "%s :End of WHOWAS"},
/* 370 */
    {0, (char *)NULL},
/* 371 */
    {RPL_INFO, ":%s"},
/* 372 */
    {RPL_MOTD, ":- %s"},
/* 373 */
    {RPL_INFOSTART, ":Server INFO"},
/* 374 */
    {RPL_ENDOFINFO, ":End of /INFO list"},
/* 375 */
    {RPL_MOTDSTART, ":- %s Mensaje de bienvenida -  "},
/* 376 */
    {RPL_ENDOFMOTD, ":End of /MOTD command"},
/* 377 */
    {0, (char *)NULL},
/* 378 */
    {0, (char *)NULL},
/* 379 */
    {RPL_WHOISMODES, "%s :Utiliza los modos [%s]"},
/* 380 */
    {0, (char *)NULL},
/* 381 */
    {RPL_YOUREOPER, ":You are now an IRC Operator"},
/* 382 */
    {RPL_REHASHING, "%s :Rehashing"},
/* 383 */
    {0, (char *)NULL},
/* 384 */
    {RPL_MYPORTIS, "%d :Port to local server is\r\n"},
/* 385 */
    {RPL_NOTOPERANYMORE, (char *)NULL},
/* 386 */
    {0, (char *)NULL},
/* 387 */
    {0, (char *)NULL},
/* 388 */
    {0, (char *)NULL},
/* 389 */
    {0, (char *)NULL},
/* 390 */
    {RPL_WHOISSUSPNICK, "%s :Tiene el nick SUSPENDido"},
/* 391 */
    {RPL_TIME, "%s " TIME_T_FMT " %ld :%s"},
/* 392 */
    {0, (char *)NULL},
/* 393 */
    {0, (char *)NULL},
/* 394 */
    {0, (char *)NULL},
/* 395 */
    {0, (char *)NULL},
/* 396 */
    {RPL_HOSTHIDDEN, "%s :is now your hidden host"},
/* 397 */
    {0, (char *)NULL},
/* 398 */
    {0, (char *)NULL},
/* 399 */
    {0, (char *)NULL},
/* 200 */
#if !defined(GODMODE)
    {RPL_TRACELINK, "Link %s%s %s %s"},
#else /* GODMODE */
    {RPL_TRACELINK, "Link %s%s %s %s " TIME_T_FMT},
#endif /* GODMODE */
/* 201 */
    {RPL_TRACECONNECTING, "Try. %d %s"},
/* 202 */
    {RPL_TRACEHANDSHAKE, "H.S. %d %s"},
/* 203 */
    {RPL_TRACEUNKNOWN, "???? %d %s"},
/* 204 */
    {RPL_TRACEOPERATOR, "Oper %d %s %ld"},
/* 205 */
    {RPL_TRACEUSER, "User %d %s %ld"},
/* 206 */
    {RPL_TRACESERVER, "Serv %d %dS %dC %s %s!%s@%s %ld %ld"},
/* 207 */
    {0, (char *)NULL},
/* 208 */
    {RPL_TRACENEWTYPE, "<newtype> 0 %s"},
/* 209 */
    {RPL_TRACECLASS, "Class %d %d"},
/* 210 */
    {0, (char *)NULL},
/* 211 */
    {RPL_STATSLINKINFO, (char *)NULL},
/* 212 */
    {RPL_STATSCOMMANDS, "%s %u %u"},
/* 213 */
    {RPL_STATSCLINE, "%c %s * %s %d %d"},
/* 214 */
    {RPL_STATSNLINE, "%c %s * %s %d %d"},
/* 215 */
    {RPL_STATSILINE, "%c %s * %s %d %d"},
/* 216 */
    {RPL_STATSKLINE, "%c %s %s %s %d %d"},
/* 217 */
    {RPL_STATSPLINE, "%c %d %d %#x"},
/* 218 */
    {RPL_STATSYLINE, "%c %d %d %d %d (%d) %ld"},
/* 219 */
    {RPL_ENDOFSTATS, "%c :End of /STATS report"},
/* 220 */
    {RPL_STATSWLINE, "%c %s@%s * %s %s :%s"},
/* 221 */
    {RPL_UMODEIS, "%s"},
/* 222 */
    {RPL_STATSJLINE, "J %s :%s"},
/* 223 */
    {RPL_STATSELINE, "%c %s * %s %d %d"},
/* 224 */
    {0, (char *)NULL},
/* 225 */
    {0, (char *)NULL},
/* 226 */
    {0, (char *)NULL},
/* 227 */
    {0, (char *)NULL},
/* 228 */
    {0, (char *)NULL},
/* 229 */
    {0, (char *)NULL},
/* 230 */
    {0, (char *)NULL},
/* 231 */
    {RPL_SERVICEINFO, (char *)NULL},
/* 232 */
    {RPL_ENDOFSERVICES, (char *)NULL},
/* 233 */
    {RPL_SERVICE, (char *)NULL},
/* 234 */
    {RPL_SERVLIST, (char *)NULL},
/* 235 */
    {RPL_SERVLISTEND, (char *)NULL},
/* 236 */
    {0, (char *)NULL},
/* 237 */
    { RPL_STATSENGINE, "%s :Event loop engine"},
/* 238 */
    {0, (char *)NULL},
/* 239 */
    {0, (char *)NULL},
/* 240 */
    {0, (char *)NULL},
/* 241 */
    {RPL_STATSLLINE, "%c %s * %s %d %d"},
/* 242 */
    {RPL_STATSUPTIME, ":Server Up %d days, %d:%02d:%02d"},
/* 243 */
    {RPL_STATSOLINE, "%c %s * %s %d %d"},
/* 244 */
    {RPL_STATSHLINE, "%c %s * %s %d %d"},
/* 245 */
    {0, (char *)NULL},
/* 246 */
    {RPL_STATSTLINE, "%c %s %s"},
/* 247 */
    {RPL_STATSGLINE, "%c %s@%s " TIME_T_FMT " :%s (expires at %s)"},
/* 248 */
    {RPL_STATSULINE, "%c %s %s %s %d %d"},
/* 249 */
    {0, (char *)NULL},
/* 250 */
    {RPL_STATSCONN, ":Highest connection count: %d (%d clients)"},
/* 251 */
    {RPL_LUSERCLIENT, ":There are %d users and %d invisible on %d servers"},
/* 252 */
    {RPL_LUSEROP, "%d :operator(s) online"},
/* 253 */
    {RPL_LUSERUNKNOWN, "%d :unknown connection(s)"},
/* 254 */
    {RPL_LUSERCHANNELS, "%d :channels formed"},
/* 255 */
    {RPL_LUSERME, ":I have %d clients and %d servers"},
/* 256 */
    {RPL_ADMINME, ":Administrative info about %s"},
/* 257 */
    {RPL_ADMINLOC1, ":%s"},
/* 258 */
    {RPL_ADMINLOC2, ":%s"},
/* 259 */
    {RPL_ADMINEMAIL, ":%s"},
/* 260 */
    {0, (char *)NULL},
/* 261 */
    {RPL_TRACELOG, "File %s %d"},
/* 262 */
    {RPL_TRACEPING, "Ping %s %s"},
/* 263 */
    {0, (char *)NULL},
/* 264 */
    {0, (char *)NULL},
/* 265 */ /* Dal.Net: Cadena numerica de usuarios locales */
    {RPL_LOCALUSERS, ":Current local users: %d Max: %d (%s, since %s)"},
/* 266 */ /* Dal.Net: Cadena numerica de usuarios globales */
    {RPL_GLOBALUSERS, ":Current global users: %d Max: %d (%s, since %s)"},
/* 267 */
    {0, (char *)NULL},
/* 268 */
    {0, (char *)NULL},
/* 269 */
    {0, (char *)NULL},
/* 270 */
    {RPL_PRIVS, "%s :%s"},
/* 271 */
    {RPL_SILELIST, "%s %s"},
/* 272 */
    {RPL_ENDOFSILELIST, "%s :End of Silence List"},
/* 273 */
    {0, (char *)NULL},
/* 274 */
    {0, (char *)NULL},
/* 275 */
    {RPL_STATSDLINE, "%c %s %s"},
/* 276 */
    {0, (char *)NULL},
/* 277 */
    {0, (char *)NULL},
/* 278 */
    {0, (char *)NULL},
/* 279 */
    {0, (char *)NULL},
/* 280 */
    {RPL_GLIST, "%s@%s " TIME_T_FMT " %s%s (expires at %s)"},
/* 281 */
    {RPL_ENDOFGLIST, ":End of G-line List"}
};

/*
 * Numericos de watch
 */
static Numeric watch_replies[] = {
/* 600 */
    {RPL_LOGON, "%s %s %s %ld :logged online"},
/* 601 */
    {RPL_LOGOFF, "%s %s %s %ld :logged offline"},
/* 602 */
    {RPL_WATCHOFF, "%s %s %s %ld :stopped watching"},
/* 603 */
#if 0  /* Desactivado */
    {RPL_WATCHSTAT, ":You have %d and are on %d WATCH entries"},
#else
    {RPL_WATCHSTAT, ":You have %d WATCH entries"},
#endif
/* 604 */
    {RPL_NOWON, "%s %s %s %ld :is online"},
/* 605 */
    {RPL_NOWOFF, "%s %s %s %ld :is offline"},
/* 606 */
    {RPL_WATCHLIST, ":%s"},
/* 607 */
    {RPL_ENDOFWATCHLIST, ":End of WATCH %c"}
};


/* *INDENT-ON* */

static char numbuff[512];

/* "inline" */
#define prepbuf(buffer, num, tail)			\
{							\
  char *s = buffer + 4;			\
  const char *ap = atoi_tab + (num << 2);	\
  							\
  strcpy(buffer, ":%s 000 %s ");			\
  *s++ = *ap++;						\
  *s++ = *ap++;						\
  *s = *ap;						\
  strcpy(s + 5, tail);					\
}

char *err_str(int numeric)
{
  Reg1 Numeric *nptr;
  Reg2 int num = numeric;

  num -= numeric_errors[0].num_val;

#if defined(DEBUGMODE)
  if (num < 0 || num > ERR_USERSDONTMATCH)
    sprintf_irc(numbuff,
        ":%%s %d %%s :INTERNAL ERROR: BAD NUMERIC! %d", numeric, num);
  else
  {
    nptr = &numeric_errors[num];
    if (!nptr->num_form || !nptr->num_val)
      sprintf_irc(numbuff, ":%%s %d %%s :NO ERROR FOR NUMERIC ERROR %d",
          numeric, num);
    else
      prepbuf(numbuff, nptr->num_val, nptr->num_form);
  }
#else
  nptr = &numeric_errors[num];
  prepbuf(numbuff, nptr->num_val, nptr->num_form);
#endif

  return numbuff;
}

char *rpl_str(int numeric)
{
  Reg1 Numeric *nptr;
  Reg2 int num = numeric;

  if (num > (int)(sizeof(local_replies) / sizeof(Numeric) - 2))
    num -= (num > 300) ? 300 : 100;

#if defined(DEBUGMODE)
  if (num < 0 || num > 200)
    sprintf_irc(numbuff, ":%%s %d %%s :INTERNAL REPLY ERROR: BAD NUMERIC! %d",
        numeric, num);
  else
  {
    if (numeric > 99)
      nptr = &numeric_replies[num];
    else
      nptr = &local_replies[num];
    Debug((DEBUG_NUM, "rpl_str: numeric %d num %d nptr %p %d %p",
        numeric, num, nptr, nptr->num_val, nptr->num_form));
    if (!nptr->num_form || !nptr->num_val)
      sprintf_irc(numbuff, ":%%s %d %%s :NO REPLY FOR NUMERIC ERROR %d",
          numeric, num);
    else
      prepbuf(numbuff, nptr->num_val, nptr->num_form);
  }
#else
  if (numeric > 99)
    nptr = &numeric_replies[num];
  else
    nptr = &local_replies[num];
  prepbuf(numbuff, nptr->num_val, nptr->num_form);
#endif

  return numbuff;
}

char *watch_str(int numeric)
{
  Reg1 Numeric *nptr;
  Reg2 int num = numeric;

  num -= watch_replies[0].num_val;

#if defined(DEBUGMODE)
  if (num < 0 || num > 7)       /* 600 - 607 */
    sprintf_irc(numbuff,
        ":%%s %d %%s :INTERNAL ERROR: BAD NUMERIC! %d", numeric, num);
  else
  {
    nptr = &watch_replies[num];
    if (!nptr->num_form || !nptr->num_val)
      sprintf_irc(numbuff, ":%%s %d %%s :NO ERROR FOR NUMERIC ERROR %d",
          numeric, num);
    else
      prepbuf(numbuff, nptr->num_val, nptr->num_form);
  }
#else
  nptr = &watch_replies[num];
  prepbuf(numbuff, nptr->num_val, nptr->num_form);
#endif

  return numbuff;
}
