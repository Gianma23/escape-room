#include <stdio.h>
#include "../../utility.h"
#include "cimitero.h"

oggetto oggetti_cimitero[] = {
    {"lucchetto", 
     "Sembra che serva una chiave per aprirlo...\n",
     "Il lucchetto è sbloccato adesso.\n",
     "C'è un **lucchetto** che lo blocca.\n",
     true, false, false, false
    },
    {"scatola",
     "Una scatola di metallo arrugginita, c'è un buco a forma di triangolo sopra.\n",
     "La scatola è aperta, al suo interno c'è una **chiave**.\n",
     "Ai suoi piedi c'è una **scatola**.\n",
     false, false, false, false
    },
    {"foglietto",
     "",
     "---------      \n"
     "         ----  \n"
     "    | | | |  --\n"
     "---------------\n",
     "All'interno c'è un **foglietto**.\n",
     false, false, false, false
    },
    {"statua",
     "Una statua di un angelo, ha le mani pronte a ricevere qualcosa.\n",
     "Ai piedi della statua si è aperto un vano con una **gemma** dentro.\n",
     "Dietro c'è una **statua**.\n",
     false, false, false, false
    },
    {"libro",
     "", 
     "Il libro è molto vecchio, c'è una **pagina_strappata**.\n",
     "Accanto c'è un **libro**.\n",
     false, false, false, false
    },
    {"pagina_strappata",
     "---------------\n"
     "    | | | |  --\n"
     "         ----  \n"
     "---------      \n"
     "(fare take per avviare l'enigma)\n",
     "---------------\n"
     "     sesso     \n"
     "---------------\n"
     "(fare take per avviare l'enigma)\n",
     "",
     true, false, true, false
    },
    {"scritta_rovinata",
     "L'oggetto ha un enigma, fare take per avviarlo.\n",
     "Enigma risolto.\n",
     "Sopra c'è una **scritta_rovinata**.\n",
     false, true, false, false
    },
    /* oggetti sbloccabili dalle use */
    {"torcia_UV",
     "",
     "Una torcia a raggi UV. È in grado di rivelare scritte nascoste.\n",
     "",
     false, false, false, true
    },
    {"spada",
     "",
     "una spada lucente\n",
     "",
     false, false, false, true
    },
    {"gemma",
     "",
     "Una gemma rossa di forma triangolare.\n",
     "",
     false, false, false, true
    },
    {"chiave",
     "",
     "blabla",
     "",
     false, false, false, true
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

/* TODO finire enigmi */
enigma enigmi_cimitero[] = {
    {"", "risposta", false}, /* enigma libro */
    {"", "risposta", false} /* enigma scritta rovinata */
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
     1, {&oggetti_cimitero[4]}},
    {"tomba",
     "La tomba è malcurata e cade a pezzi. ",
     1, {&oggetti_cimitero[5]}},
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
