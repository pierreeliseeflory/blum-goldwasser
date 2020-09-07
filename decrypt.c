#include "Blum.h"

int main(int argc, char *argv[]) {
    // à compléter
    /*
    1) lire un fichier chiffré dont le chemin est donné en argument (utiliser la
    fonction read_cipher). 2) lire la clé privée dont le chemin est donné en
    argument (utiliser la fonction read_prvt). 3) déchiffrer le message avec
    l'algorithme de Blum-Goldwasser. 4) écrire le texte clair dans un fichier
    (utiliser la fonction write_plain).
    */

    if (argc != 4) {
        printf("Nombre d'arguments decrypt.c incorrect.\n");
        return EXIT_FAILURE;
    }

    // 1
    mpz_t pub;
    mpz_init(pub);
    read_pub(pub, "key.pub");
    size_t sizeOfPublicKey = mpz_sizeinbase(pub, 2);
    mpz_clear(pub);
    // printf("%ld\n", sizeOfPublicKey);

    char *cipher;
    unsigned int size;
    size = read_cipher(argv[1], &cipher);
    size -= (sizeOfPublicKey + 2);

    int offset = 0;
    while (cipher[size + offset] == 0) offset++;
    // printf("Offset: %d\n", offset);

    char *lastXValue = NULL;
    lastXValue = malloc((sizeOfPublicKey + 3 - offset) * sizeof(char));
    for (int i = 0; i < (sizeOfPublicKey + 2 - offset); i++)
        lastXValue[i] = (cipher[size + offset + i] == 1) ? 49 : 48;
    lastXValue[sizeOfPublicKey + 2 - offset] = '\0';

    mpz_t xEnd;
    mpz_init(xEnd);
    mpz_set_str(xEnd, lastXValue, 2);
    // mpz_out_str(NULL, 10, xEnd);
    // printf("\n");

    // 2
    mpz_t p, q;
    mpz_inits(p, q, NULL);
    read_prvt(p, q, argv[2]);

    // 3
    // A ce niveau on a les bonnes valeurs de Xt+1, p et q
    char *plain;
    mpz_t d1, d2, t1, t2, u, v, a, b, x, n;
    mpz_inits(d1, d2, t1, t2, u, v, a, b, x, n, NULL);

    mpz_add_ui(t1, p, 1);
    mpz_add_ui(t2, q, 1);
    mpz_divexact_ui(d1, t1, 4);
    mpz_divexact_ui(d2, t2, 4);

    mpz_sub_ui(p, p, 1);
    mpz_powm_ui(t1, d1, size + 1, p);
    mpz_sub_ui(q, q, 1);
    mpz_powm_ui(t2, d2, size + 1, q);

    mpz_add_ui(p, p, 1);
    mpz_add_ui(q, q, 1);

    mpz_powm(u, xEnd, t1, p);
    mpz_powm(v, xEnd, t2, q);

    mpz_invert(a, p, q);
    mpz_invert(b, q, p);

    // test de la fonction mpz_invert
    // mpz_mul(t1, a, p);
    // mpz_mod(t1, t1, q);
    // mpz_out_str(NULL, 10, t1);

    mpz_mul(n, p, q);

    mpz_mul(u, u, b);
    mpz_mul(u, u, q);
    mpz_mul(v, v, a);
    mpz_mul(v, v, p);

    mpz_add(x, u, v);
    // mpz_out_str(NULL, 10, n);
    mpz_mod(x, x, n);
    // mpz_out_str(NULL, 10, x);

    char z;
    plain = malloc(size * sizeof(char));
    for (int i = 0; i < size; i++) {
        z = BBS_step(n, x);
        // printf("%d\n", z);
        plain[i] = cipher[i] ^ z;
    }

    mpz_clears(xEnd, p, q, d1, d2, t1, t2, u, v, a, b, x, n, NULL);

    // 4
    write_plain(argv[3], plain, size);

    exit(EXIT_SUCCESS);
}
