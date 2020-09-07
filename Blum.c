#include "Blum.h"

// Declare our random state
gmp_randstate_t state;
int initialized = 0;

// int Fermat(const mpz_t n, unsigned int k, gmp_randstate_t state) {
//     mpz_t a, bound, e, tmp;
//     mpz_init(tmp);
//     mpz_init(a);
//     mpz_init_set(bound, n);
//     mpz_sub_ui(bound, bound, 3);
//     mpz_init_set(e, n);
//     mpz_sub_ui(e, e, 1);

//     for (; k > 0; --k) {
//         mpz_urandomm(a, state, bound);
//         mpz_add_ui(a, a, 2);  // 1 < a < n-1
//         mpz_powm(tmp, a, e, n);
//         if (mpz_cmp_ui(tmp, 1)) {
//             mpz_clear(a);
//             mpz_clear(bound);
//             mpz_clear(e);
//             mpz_clear(tmp);
//             return 0;
//         }
//     }
//     mpz_clear(a);
//     mpz_clear(bound);
//     mpz_clear(e);
//     mpz_clear(tmp);
//     return 1;
// }

// void GenPremier(mpz_t res, const mpz_t n, unsigned int k,
//                 gmp_randstate_t state) {
//     mpz_t rest;
//     mpz_init(rest);
//     while (1) {
//         mpz_urandomm(res, state, n);
//         if (Fermat(res, k, state)) {
//             mpz_mod_ui(rest, res, 4);
//             if (!mpz_cmp_ui(rest, 3)) {
//                 mpz_clear(rest);
//                 return;
//             }
//         }
//     }
// }

// J'utilise les fonctions que j'ai créé dans le tp6
bool Fermat(mpz_t n) {
    mpz_t a, res, exp;
    bool isPrime;
    mpz_inits(a, res, exp, NULL);

    // Generate a candidate a
    mpz_urandomm(a, state, n);

    mpz_sub_ui(exp, n, 1);
    mpz_powm(res, a, exp, n);

    isPrime = mpz_cmp_ui(res, 1) == 0;

    // Clears variables
    mpz_clears(a, exp, res, NULL);
    return isPrime;
};

// Takes mpz by address because we can't return it otherwise
void GenPremier(int size, mpz_t *res) {
    // Nécessaire pour ne pas réinitialiser la germe aléatoire à chaque fois et
    // obtenir 2 entiers identiques en clé privée
    if (initialized == 0) {
        gmp_randinit_default(state);
        initialized = 1;
    }

    // Creating our mpz elements to compare with
    mpz_t a, b;
    mpz_init(a);
    mpz_add_ui(a, a, 3);
    mpz_init(b);
    mpz_add_ui(b, b, 4);

    // While we don't have a prime that is congruent to 3 mod 4, choose another
    // one
    do {
        mpz_rrandomb(*res, state, size);
    } while (!(Fermat(*res) && (mpz_congruent_p(*res, a, b))));

    // Clear our temporary elements
    mpz_clear(a);
    mpz_clear(b);
};

char BBS_step(const mpz_t n, mpz_t x) {
    // prend en entrée xi et la valeur de n, calcule xi+1 et renvoi zi+1
    char res = 1;

    mpz_mul(x, x, x);
    mpz_mod(x, x, n);

    if (mpz_even_p(x) == 0) res = 0;

    return res;
}

void read_pub(mpz_t pub, char *file_path) {
    FILE *fd = fopen(file_path, "r");
    if (fd == NULL) {
        fprintf(stderr, "Can't open %s\n", file_path);
        exit(EXIT_FAILURE);
    }
    char str[2048];
    fgets(str, 2048, fd);
    mpz_init_set_str(pub, str, 10);
    fclose(fd);
    return;
}

void write_pub(char *file_path, mpz_t pub) {
    FILE *fd = fopen(file_path, "w");
    if (fd == NULL) {
        fprintf(stderr, "Can't open %s\n", file_path);
        exit(EXIT_FAILURE);
    }
    gmp_fprintf(fd, "%Zd", pub);
    fclose(fd);
    return;
}

void read_prvt(mpz_t prvt1, mpz_t prvt2, char *file_path) {
    FILE *fd = fopen(file_path, "r");
    if (fd == NULL) {
        fprintf(stderr, "Can't open %s\n", file_path);
        exit(EXIT_FAILURE);
    }
    char str[1024];
    fgets(str, 1024, fd);
    mpz_init_set_str(prvt1, str, 10);
    fgets(str, 1024, fd);
    mpz_init_set_str(prvt2, str, 10);
    fclose(fd);
    return;
}

void write_prvt(char *file_path, mpz_t prvt1, mpz_t prvt2) {
    FILE *fd = fopen(file_path, "w");
    if (fd == NULL) {
        fprintf(stderr, "Can't open %s\n", file_path);
        exit(EXIT_FAILURE);
    }
    gmp_fprintf(fd, "%Zd\n%Zd", prvt1, prvt2);
    fclose(fd);
    return;
}

unsigned int read_plain(char *file_path, char **plain) {
    FILE *fd = fopen(file_path, "r");
    if (fd == NULL) {
        fprintf(stderr, "Can't open %s\n", file_path);
        exit(EXIT_FAILURE);
    }
    // memory allocation
    fseek(fd, 0, SEEK_END);
    int length = ftell(fd);
    rewind(fd);
    char *tmp = malloc(8 * length * sizeof(char));
    if (tmp == NULL) return 0;
    ////////////////////
    char c;
    unsigned int i = 0, j;
    while ((c = fgetc(fd)) != EOF) {
        for (j = 0; j < 8; ++j) {
            tmp[i++] = ((c & 0b10000000) ? 1 : 0);
            c <<= 1;
        }
    }
    fclose(fd);
    *plain = tmp;

    return i;
}

void write_plain(char *file_path, char *plain, unsigned int size) {
    FILE *fd = fopen(file_path, "w");
    if (fd == NULL) {
        fprintf(stderr, "Can't open %s\n", file_path);
        exit(EXIT_FAILURE);
    }
    char c;
    unsigned int i = 0, j;
    for (; i < size;) {
        c = 0;
        for (j = 0; j < 8; ++j) {
            c <<= 1;
            c ^= (plain[i++] ? 1 : 0);
        }
        putc(c, fd);
    }
    fclose(fd);
    return;
}

unsigned int read_cipher(char *file_path, char **cipher) {
    FILE *fd = fopen(file_path, "r");
    if (fd == NULL) {
        fprintf(stderr, "Can't open %s\n", file_path);
        exit(EXIT_FAILURE);
    }
    // memory allocation
    fseek(fd, 0, SEEK_END);
    int length = ftell(fd);
    fseek(fd, 0, SEEK_SET);
    char *tmp = malloc(length * sizeof(char));
    if (tmp == NULL) return 0;
    ////////////////////
    char c;
    unsigned int i = 0;
    while ((c = fgetc(fd)) != EOF) {
        tmp[i++] = (c == '1' ? 1 : 0);
    }
    fclose(fd);
    printf("size = %d\n", i);
    *cipher = tmp;
    return i;
}

void write_cipher(char *file_path, char *cipher, unsigned int size) {
    FILE *fd = fopen(file_path, "w");
    if (fd == NULL) {
        fprintf(stderr, "Can't open %s\n", file_path);
        exit(EXIT_FAILURE);
    }
    unsigned int i = 0;
    for (; i < size; ++i) {
        putc((cipher[i] ? '1' : '0'), fd);
    }
    fclose(fd);
    return;
}
