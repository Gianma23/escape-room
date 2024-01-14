#include <stdio.h>
#include "../../utility.h"
#include "cimitero.h"

oggetto oggetti_cimitero[];
/* TODO finire enigmi */
enigma enigmi_cimitero[] = {
    {"Scopri la parola scritta sulla pagina:\n", "crepuscolo", false, /* enigma libro */
     "Hai indovinato la parola! Sull'altare si è aperto un cassetto.\n",
     &oggetti_cimitero[7]}, 
    {"Indovinello:\n", "la spada", false, /* enigma scritta rovinata */
     "Bravo, hai indovinato! La terra si smuove e vedi qualcosa sbucare dalla tomba.\n",
     &oggetti_cimitero[8]} 
};

oggetto oggetti_cimitero[] = {
    {"lucchetto", 
     "Sembra che serva una chiave per aprirlo...\n",
     "Il lucchetto è sbloccato adesso.\n",
     "C'è un **lucchetto** che lo blocca.\n",
     true, false, false, NULL
    },
    {"scatola",
     "Una scatola di metallo arrugginita, c'è un buco a forma di triangolo sopra.\n",
     "La scatola è aperta, al suo interno c'è una **chiave**.\n",
     "Ai suoi piedi c'è una **scatola**.\n",
     true, false, false, NULL
    },
    {"foglietto",
     "",
     "-----------\n"
     " -       - \n"
     "  -p*sc*-  \n"
     "   -----   \n",
     "All'interno c'è un **foglietto**.\n",
     false, false, false, NULL
    },
    {"statua",
     "Una statua di un angelo, ha le mani pronte a ricevere qualcosa.\n",
     "Ai piedi della statua si è aperto un vano con una **gemma** dentro.\n",
     "Dietro c'è una **statua**.\n",
     true, false, false, NULL
    },
    {"libro",
     "", 
     "Il libro è molto vecchio, c'è una **pagina_strappata**.\n",
     "Accanto c'è un **libro**.\n",
     true, false, false, NULL
    },
    {"pagina_strappata",
     "|------         ---|\n"
     "|      --    ---   |\n"
     "|    cr*--  -lo    |\n"
     "|         --       |\n"
     "|          -       |\n"
     "|------------------|\n",
     "|------------------|\n"
     "|                  |\n"
     "|    cr*p*sc*lo    |\n"
     "|                  |\n"
     "|                  |\n"
     "|------------------|\n",
     "",
     true, false, false, &enigmi_cimitero[0]
    },
    {"scritta_rovinata",
     "Ricordati i giorni nei quali fui %/a/.,\n"
     "!n|an:_ui**t* e battuta...maltrattata.\n"
     "La %/*_* mi circondava, e poco importava,\n"
     "poiché qualunque fosse il _.<*r* che infliggevo, non m/([re* mai.\n",
     "Ricordati i giorni nei quali fui usata,\n"
     "insanguinata e battuta...maltrattata.\n"
     "La morte mi circondava, e poco importava,\n"
     "poiché qualunque fosse il dolore che infliggevo, non marcirei mai.\n",
     "Sopra c'è una **scritta_rovinata**.\n",
     true, false, false, &enigmi_cimitero[1]
    },
    /* oggetti sbloccabili dalle use */
    {"torcia_UV",
     "",
     "Una torcia a raggi UV. È in grado di rivelare scritte nascoste.\n",
     "C'è un cassetto aperto, all'interno c'è una **torcia_UV**.\n",
     false, false, true, NULL
    },
    {"spada",
     "",
     "Una spada lucente, è molto pesante.\n",
     "Dalla terra spunta il manico di una **spada**.\n",
     false, false, true, NULL
    },
    {"gemma",
     "",
     "Una gemma rossa di forma triangolare.\n",
     "",
     false, false, true, NULL
    },
    {"chiave",
     "",
     "Una chiave argentata.\n",
     "",
     false, false, true, NULL
    }
};

utilizzo utilizzi_cimitero[] = {
    {"foglietto", "pagina_strappata",
     "I due pezzi combaciano, ora puoi leggere la pagina completa.\n",
     NULL, false
    },
    {"torcia_UV", "scritta_rovinata",
     "La torcia ha rivelato le lettere mancanti, ora puoi leggere la scritta completa.\n",
     NULL, false
    },
    {"spada", "statua", 
     "Posi la spada sulle mani della statua. Si apre un vano ai suoi piedi.\n",
     NULL, true
    },
    {"gemma", "scatola",
     "La gemma si incastra perfettamente nel buco nella scatola, sembra che si sia aperta.\n",
     &oggetti_cimitero[6], true
    },
    {"chiave", "lucchetto",
     "Il lucchetto si è aperto!\n",
      NULL, true
    }
};

locazione locazioni_cimitero[] = {
    {"cancello",
     "È l'unica via d'uscita. ",
     1, {&oggetti_cimitero[0]}},
    {"albero",
     "L'albero è secco e nodoso. Nel suo tronco vedi un ++buco++. ",
     1, {&oggetti_cimitero[1]}},
    {"buco",
     "Riesci a malapena a vedere cosa ci sia dentro. ",
     1, {&oggetti_cimitero[2]}},
    {"chiesa",
     "La chiesa all'interno è tetra e fai fatica a vedere. Intravedi un ++altare++. ",
     1, {&oggetti_cimitero[3]}},
    {"altare",
     "L'altare è illuminato da una fioca luce. ",
     2, {&oggetti_cimitero[4], &oggetti_cimitero[7]}},
    {"tomba",
     "La tomba è malcurata e cade a pezzi. ",
     1, {&oggetti_cimitero[5]} /* TODO mettere spada */},
};

scenario scenario_cimitero = {
    "cimitero",
    "Ti trovi in un cimitero, l'unica uscita è un ++cancello++. Alla\n"
    "sua sinistra vedi un ++albero++ secco, lì accanto è presente una\n"
    "++tomba++. Dietro di te c'è una ++chiesa++.\n",
    N_OGGETTI_CIMITERO, 
    N_LOCAZIONI_CIMITERO,
    N_UTILIZZI_CIMITERO,
    oggetti_cimitero,
    locazioni_cimitero,
    utilizzi_cimitero,
    N_TOKEN_CIMITERO
};
