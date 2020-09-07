#include "Blum.h"

int main(int argc, char *argv[]) {
    // à compléter
    /*
    génèrer la clé privée et la clé publique et les écrire respectivement dans
    les fichiers key.pub et key.prvt (utiliser les fonctions write_pub et
    write_prvt). On pourra éventuellement donner la taille des clés en argument.
    */
    if (argc > 2) {
        printf("Trop d'arguments pour genkey.c\n");
        return (EXIT_FAILURE);
    }

    size_t sizeKey = 2048;

    if (argc == 2) sizeKey = atoi(argv[1]);

    mpz_t p, q, n;
    mpz_inits(p, q, n, NULL);
    GenPremier(sizeKey, &p);
    GenPremier(sizeKey, &q);
    mpz_mul(n, p, q);

    // writes key.pub and key.prvt
    write_pub("key.pub", n);
    write_prvt("key.prvt", p, q);

    mpz_clears(p, q, n, NULL);

    return (EXIT_SUCCESS);
}
