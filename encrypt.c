#include "Blum.h"

int main(int argc, char *argv[]) {
    // à compléter
    /*
    1) lire un fichier clair dont le chemin est donné en argument (utiliser la
    fonction read_plain). 2) lire une clé publique dont le chemin est donné en
    argument (utiliser la fonction read_pub). 3) chiffrer le message avec
    l'algorithme de Blum-Goldwasser. 4) écrire le texte chiffré dans un fichier
    (utiliser la fonction write_cipher).
    */
    //  message - key.pub - cipher
    if (argc != 4) {
        printf("Nombre d'arguments encrypt.c incorrect.\n");
        return EXIT_FAILURE;
    }
    // 1
    unsigned int size;
    char *plain;

    size = read_plain(argv[1], &plain);

    // 2
    mpz_t pub;
    mpz_init(pub);
    read_pub(pub, argv[2]);
    size_t sizeOfPublicKey = mpz_sizeinbase(pub, 2);
    // printf("Taille de la cle publique: %ld\n", sizeOfPublicKey);

    // 3
    char *cipher, bit;
    mpz_t germe, x;
    mpz_inits(germe, x, NULL);

    gmp_randstate_t state;
    gmp_randinit_default(state);

    mpz_urandomm(germe, state, pub);
    mpz_mul(x, germe, germe);
    mpz_mod(x, x, pub);

    cipher = malloc((size + (sizeOfPublicKey + 2)) *
                    sizeof(char));  // pas de clé plus grande que 2048 bits

    bit = BBS_step(pub, x);
    for (int i = 0; i < size; i++) {
        cipher[i] = bit ^ plain[i];
        bit = BBS_step(pub, x);
    }

    char *tmp = NULL;
    tmp = malloc((sizeOfPublicKey + 2) *
                 sizeof(char));  // comprend '/0' et le signe
    mpz_get_str(tmp, 2, x);
    int offset = 0;
    for (int j = 0; j < sizeOfPublicKey + 2; j++)
        if (tmp[j] == '\0') {
            offset = j;
            break;
        }

    offset = sizeOfPublicKey + 2 - offset;
    // printf("Offset: %d\n", offset);

    for (size_t i = 0; i < offset; i++) {
        cipher[size + i] = 0;
    }

    for (int i = 0; i < (sizeOfPublicKey + 2 - offset); i++) {
        if (tmp[i] == 49)
            cipher[size + offset + i] = 1;
        else
            cipher[size + offset + i] = 0;
    }

    // mpz_set_str(x, tmp, 2);
    // mpz_out_str(NULL, 10, x);
    // printf("%s", tmp);
    // printf("\n");

    mpz_clears(pub, germe, x, NULL);

    // 4
    write_cipher(argv[3], cipher, size + (sizeOfPublicKey + 2));

    exit(EXIT_SUCCESS);
}
