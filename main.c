#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

typedef struct {
    unsigned int x, y, culoare, latime, inaltime;
    double corelatie;
    _Bool use;
}
fereastra;
void xorshift32(unsigned int seed, unsigned int n, unsigned int ** R) {
    unsigned int r, i;
    r = seed;
    * R = (unsigned int * ) malloc(n * sizeof(unsigned int));
    for (i = 0; i < n; i++) {
        r = (r ^ r << 13);
        r = (r ^ r >> 17);
        r = (r ^ r << 5);
        ( * R)[i] = r;
    }
}
void Permutare(unsigned int n, unsigned int ** P, unsigned int * R) {
    unsigned int i, aux, r;
    * P = (unsigned int * ) malloc(n * sizeof(unsigned int));
    for (i = 0; i < n; i++) {
        ( * P)[i] = i;
    }
    for (i = n - 1; i >= 1; i--) {
        r = R[n - i - 1] % (i + 1);
        aux = ( * P)[r];
        ( * P)[r] = ( * P)[i];
        ( * P)[i] = aux;
    }
}
void Liniarizare(char * nume_fisier_sursa, unsigned int * dim_img, unsigned int * latime_img, unsigned int * inaltime_img, unsigned int * padding, unsigned int ** * L, unsigned char ** Header) {
    FILE * fin;
    fin = fopen(nume_fisier_sursa, "rb");
    if (fin == NULL) {
        printf("nu am gasit imaginea sursa din care citesc");
        return;
    }
    fseek(fin, 2, SEEK_SET);
    fread( & ( * dim_img), sizeof(unsigned int), 1, fin);
    fseek(fin, 18, SEEK_SET);
    fread( & ( * latime_img), sizeof(unsigned int), 1, fin);
    fread( & ( * inaltime_img), sizeof(unsigned int), 1, fin);
    fseek(fin, 0, SEEK_SET);
    unsigned int i, j;
    if (( * latime_img) % 4 != 0) {
        * padding = 4 - (3 * ( * latime_img)) % 4;
    } else {
        * padding = 0;
    }
    * Header = (unsigned char * ) malloc(54 * sizeof(unsigned char));
    for (i = 0; i < 54; i++) {
        fread( & ( * Header)[i], sizeof(unsigned char), 1, fin);
    }
    * L = (unsigned int ** ) malloc(( * latime_img) * ( * inaltime_img) * sizeof(unsigned int * ));
    for (i = 0; i < ( * latime_img) * ( * inaltime_img); i++) {
        ( * L)[i] = calloc(3, sizeof(unsigned int));
    }
    for (i = 0; i < * inaltime_img; i++) {
        for (j = 0; j < * latime_img; j++) {
            fread( & ( * L)[( * inaltime_img - i - 1) * ( * latime_img) + j][0], 1, 1, fin);
            fread( & ( * L)[( * inaltime_img - i - 1) * ( * latime_img) + j][1], 1, 1, fin);
            fread( & ( * L)[( * inaltime_img - i - 1) * ( * latime_img) + j][2], 1, 1, fin);
        }
        fseek(fin, * padding, SEEK_CUR);
    }
    fclose(fin);
}
void Afisare(char * nume_fisier_sursa, unsigned char * Header, unsigned int ** IMG, unsigned int latime_img, unsigned int inaltime_img, unsigned int padding) {
    FILE * fout;
    fout = fopen(nume_fisier_sursa, "wb");
    unsigned char bmppad[3] = {0,0,0};
    unsigned int i, j;
    for (i = 0; i < 54; i++) {
        fwrite( & Header[i], 1, 1, fout);
    }
    for (i = 0; i < inaltime_img; i++) {
        for (j = 0; j < latime_img; j++) {
            fwrite( & IMG[(inaltime_img - i - 1) * latime_img + j][0], 1, 1, fout);
            fwrite( & IMG[(inaltime_img - i - 1) * latime_img + j][1], 1, 1, fout);
            fwrite( & IMG[(inaltime_img - i - 1) * latime_img + j][2], 1, 1, fout);
        }
        fwrite(bmppad, 1, padding, fout);
    }
    fclose(fout);
}
void Criptare(char * nume_fisier_sursa, unsigned int ** L, unsigned int *** LP, unsigned int *** C, unsigned int * R, unsigned int * P, int latime_img, unsigned int inaltime_img) {
    FILE * fin;
    fin = fopen(nume_fisier_sursa, "r");
    unsigned int SV, R0, nr, X, i, j, k, V;
    fscanf(fin, "%u %u", & R0, & SV);
    fclose(fin);
    * LP = (unsigned int ** ) malloc(latime_img * inaltime_img * sizeof(unsigned int * ));
    for (i = 0; i < latime_img * inaltime_img; i++) {
        ( * LP)[i] = calloc(3, sizeof(unsigned int));
    }
    nr = 0;
    for (i = 0; i < inaltime_img; i++) {
        for (j = 0; j < latime_img; j++) {
            ( * LP)[P[nr]][0] = L[nr][0];
            ( * LP)[P[nr]][1] = L[nr][1];
            ( * LP)[P[nr]][2] = L[nr][2];
            nr++;
        }
    }
    * C = (unsigned int ** ) malloc(latime_img * inaltime_img * sizeof(unsigned int * ));
    for (i = 0; i < latime_img * inaltime_img; i++) {
        ( * C)[i] = calloc(3, sizeof(unsigned int));
    }
    nr = 0;
    for (i = 0; i < inaltime_img; i++) {
        for (j = 0; j < latime_img; j++) {
            V = R[inaltime_img * latime_img + nr - 1];
            if (nr == 0) {
                for (k = 0; k < 3; k++) {
                    X = SV & 255;
                    SV = SV >> 8;
                    ( * C)[nr][k] = X ^ ( * LP)[nr][k];
                }
                for (k = 0; k < 3; k++) {
                    X = V & 255;
                    V = V >> 8;
                    ( * C)[nr][k] = ( * C)[nr][k] ^ X;
                }
            } else {
                for (k = 0; k < 3; k++) {
                    ( * C)[nr][k] = ( * C)[nr - 1][k] ^ ( * LP)[nr][k];
                }
                for (k = 0; k < 3; k++) {
                    X = V & 255;
                    V = V >> 8;
                    ( * C)[nr][k] = ( * C)[nr][k] ^ X;
                }
            }
            nr++;
        }
    }
}
void Decriptare(char * nume_fisier_sursa, unsigned int *** D, unsigned int ** C, unsigned int * P, unsigned int * R, int latime_img, unsigned int inaltime_img) {
    FILE * fin;
    fin = fopen(nume_fisier_sursa, "r");
    unsigned int SV, R0, nr, X, i, j, k, V, ** CP, * INV;
    fscanf(fin, "%u %u", & R0, & SV);
    fclose(fin);
    INV = (unsigned int * ) malloc(inaltime_img * latime_img * sizeof(unsigned int));
    for (i = 0; i < inaltime_img * latime_img; i++) {
        INV[P[i]] = i;
    }
    CP = (unsigned int ** ) malloc(latime_img * inaltime_img * sizeof(unsigned int * ));
    for (i = 0; i < latime_img * inaltime_img; i++) {
        CP[i] = calloc(3, sizeof(unsigned int));
    }
    nr = 0;
    for (i = 0; i < inaltime_img; i++) {
        for (j = 0; j < latime_img; j++) {
            V = R[inaltime_img * latime_img + nr - 1];
            if (nr == 0) {
                for (k = 0; k < 3; k++) {
                    X = SV & 255;
                    SV = SV >> 8;
                    CP[nr][k] = X ^ C[nr][k];
                }
                for (k = 0; k < 3; k++) {
                    X = V & 255;
                    V = V >> 8;
                    CP[nr][k] = CP[nr][k] ^ X;
                }
            } else {
                for (k = 0; k < 3; k++) {
                    CP[nr][k] = C[nr - 1][k] ^ C[nr][k];
                }
                for (k = 0; k < 3; k++) {
                    X = V & 255;
                    V = V >> 8;
                    CP[nr][k] = CP[nr][k] ^ X;
                }
            }
            nr++;
        }
    }
    ( * D) = (unsigned int ** ) malloc(latime_img * inaltime_img * sizeof(unsigned int * ));
    for (i = 0; i < latime_img * inaltime_img; i++) {
        ( * D)[i] = calloc(3, sizeof(unsigned int));
    }
    for (i = 0; i < inaltime_img * latime_img; i++) {
        ( * D)[INV[i]][0] = CP[i][0];
        ( * D)[INV[i]][1] = CP[i][1];
        ( * D)[INV[i]][2] = CP[i][2];
    }
    free(INV);
    for (i = 0; i < inaltime_img * latime_img; i++) {
        free(CP[i]);
    }
    free(CP);
}
void Test_Chi_Patrat(unsigned int ** IMG, unsigned int latime_img, unsigned int inaltime_img) {
    double frecv, * frecvB, * frecvG, * frecvR, B, G, R;
    unsigned int i;
    frecv = (inaltime_img * latime_img) / 256.0;
    frecvB = (double * ) malloc(260 * sizeof(double));
    frecvG = (double * ) malloc(260 * sizeof(double));
    frecvR = (double * ) malloc(260 * sizeof(double));
    for (i = 0; i < 256; i++) {
        frecvB[i] = frecvG[i] = frecvR[i] = 0;
    }
    for (i = 0; i < latime_img * inaltime_img; i++) {
        frecvB[IMG[i][0]]++;
        frecvG[IMG[i][1]]++;
        frecvR[IMG[i][2]]++;
    }
    B = G = R = 0;
    for (i = 0; i < 256; i++) {
        B = B + (frecvB[i] - frecv) * (frecvB[i] - frecv) / frecv;
        G = G + (frecvG[i] - frecv) * (frecvG[i] - frecv) / frecv;
        R = R + (frecvR[i] - frecv) * (frecvR[i] - frecv) / frecv;
    }
    printf("R: %lf\n", R);
    printf("G: %lf\n", G);
    printf("B: %lf\n", B);
    free(frecvB);
    free(frecvG);
    free(frecvR);
}
void Imagine_Grayscale(char * nume_fisier_sursa, unsigned int ** L, unsigned int latime_img, unsigned int inaltime_img, unsigned int padding) {
    FILE * fin;
    unsigned char pRGB[3], aux;
    fin = fopen(nume_fisier_sursa, "rb");
    if (fin == NULL) {
        printf("nu am gasit imaginea sursa din care citesc");
        return;
    }
    fseek(fin, 54, SEEK_SET);
    int i, j;
    for (i = 0; i < inaltime_img; i++) {
        for (j = 0; j < latime_img; j++) {
            fread(pRGB, 3, 1, fin);
            aux = 0.299 * pRGB[2] + 0.587 * pRGB[1] + 0.114 * pRGB[0];
            L[(inaltime_img - i - 1) * latime_img + j][0] = aux;
            L[(inaltime_img - i - 1) * latime_img + j][1] = aux;
            L[(inaltime_img - i - 1) * latime_img + j][2] = aux;
        }
        fseek(fin, padding, SEEK_CUR);
    }
    fclose(fin);
}
void Colorare(unsigned int ** L, unsigned char col[3], unsigned int x, unsigned int y, unsigned int latime_img, unsigned int inaltime_img, int latime_fereastra, unsigned int inaltime_fereastra) {
    int l, k;
    for (l = 0; l < latime_fereastra; l++) {
        L[latime_img * x + y + l][0] = col[2];
        L[latime_img * x + y + l][1] = col[1];
        L[latime_img * x + y + l][2] = col[0];
        L[latime_img * (x + inaltime_fereastra - 1) + y + l][0] = col[2];
        L[latime_img * (x + inaltime_fereastra - 1) + y + l][1] = col[1];
        L[latime_img * (x + inaltime_fereastra - 1) + y + l][2] = col[0];
    }
    for (k = 0; k < inaltime_fereastra; k++) {
        L[latime_img * (x + k) + y][0] = col[2];
        L[latime_img * (x + k) + y][1] = col[1];
        L[latime_img * (x + k) + y][2] = col[0];
        L[latime_img * (x + k) + y + latime_fereastra - 1][0] = col[2];
        L[latime_img * (x + k) + y + latime_fereastra - 1][1] = col[1];
        L[latime_img * (x + k) + y + latime_fereastra - 1][2] = col[0];
    }
}
fereastra * Detectii(char * nume_fisier_sursa, unsigned int ** L, unsigned int latime_img, unsigned int inaltime_img, unsigned int padding, double pg, unsigned int culoare, unsigned int * nr) {
    FILE * fin;
    unsigned int latime_sablon, inaltime_sablon, padding_sablon, dim_sablon, i, j, ** S, k, l;
    fereastra * F, * aux;
    fin = fopen(nume_fisier_sursa, "rb");
    fseek(fin, 2, SEEK_SET);
    fread( & dim_sablon, sizeof(unsigned int), 1, fin);
    fseek(fin, 18, SEEK_SET);
    fread( & latime_sablon, sizeof(unsigned int), 1, fin);
    fread( & inaltime_sablon, sizeof(unsigned int), 1, fin);
    fseek(fin, 0, SEEK_SET);
    if (latime_sablon % 4 != 0) {
        padding_sablon = 4 - (3 * (latime_sablon)) % 4;
    } else {
        padding_sablon = 0;
    }
    S = (unsigned int ** ) malloc(latime_sablon * inaltime_sablon * sizeof(unsigned int * ));
    for (i = 0; i < latime_sablon * inaltime_sablon; i++) {
        S[i] = calloc(3, sizeof(unsigned int));
    }
    fseek(fin, 54, SEEK_SET);
    for (i = 0; i < inaltime_sablon; i++) {
        for (j = 0; j < latime_sablon; j++) {
            fread( & S[(inaltime_sablon - i - 1) * (latime_sablon) + j][0], 1, 1, fin);
            fread( & S[(inaltime_sablon - i - 1) * (latime_sablon) + j][1], 1, 1, fin);
            fread( & S[(inaltime_sablon - i - 1) * (latime_sablon) + j][2], 1, 1, fin);
        }
        fseek(fin, padding_sablon, SEEK_CUR);
    }
    double corelatie, sbar, devstandard, devstandardf, fbar;
    sbar = 0;
    devstandard = 0;
    for (k = 0; k < inaltime_sablon; k++) {
        for (l = 0; l < latime_sablon; l++) {
            sbar = sbar + S[latime_sablon * k + l][0];
        }
    }
    sbar = sbar / 165.0;
    devstandard = 0;
    for (k = 0; k < inaltime_sablon; k++) {
        for (l = 0; l < latime_sablon; l++) {
            devstandard = devstandard + (S[latime_sablon * k + l][0] - sbar) * (S[latime_sablon * k + l][0] - sbar);
        }
    }
    devstandard = devstandard / 164.0;
    devstandard = sqrt(devstandard);
    F = (fereastra * ) malloc(sizeof(fereastra));
    * nr = 0;
    for (i = 0; i < inaltime_img; i++) {
        for (j = 0; j < latime_img; j++) {
            if (i + inaltime_sablon - 1 < inaltime_img && j + latime_sablon - 1 < latime_img) {
                corelatie = 0;
                fbar = 0;
                for (k = 0; k < inaltime_sablon; k++) {
                    for (l = 0; l < latime_sablon; l++) {
                        fbar = fbar + L[latime_img * (i + k) + j + l][0];
                    }
                }
                fbar = fbar / 165.0;
                devstandardf = 0;
                for (k = 0; k < inaltime_sablon; k++) {
                    for (l = 0; l < latime_sablon; l++) {
                        devstandardf = devstandardf + (L[latime_img * (i + k) + j + l][0] - fbar) * (L[latime_img * (i + k) + j + l][0] - fbar);
                    }
                }
                devstandardf = devstandardf / 164.0;
                devstandardf = sqrt(devstandardf);

                for (k = 0; k < inaltime_sablon; k++) {
                    for (l = 0; l < latime_sablon; l++) {
                        corelatie = corelatie + (L[latime_img * (i + k) + j + l][0] - fbar) * (S[latime_sablon * k + l][0] - sbar) / (devstandard * devstandardf);
                    }
                }
                corelatie = corelatie / 165.0;
                if (corelatie >= pg) {
                    F[ * nr].x = i;
                    F[ * nr].y = j;
                    F[ * nr].culoare = culoare;
                    F[ * nr].corelatie = corelatie;
                    F[ * nr].latime = latime_sablon;
                    F[ * nr].inaltime = inaltime_sablon;
                    F[ * nr].use = 1;
                    * nr = * nr + 1;
                    aux = (fereastra * ) realloc(F, ( * nr + 1) * sizeof(fereastra));
                    F = aux;
                }
            }
        }
    }
    for (i = 0; i < inaltime_sablon * latime_sablon; i++) {
        free(S[i]);
    }
    free(S);
    return F;
}
int cmp(const void * a,const void * b) {
    fereastra * x = (fereastra * ) a;
    fereastra * y = (fereastra * ) b;
    if (x->corelatie > y->corelatie) {
        return -1;
    } else {
        return 1;
    }
}
double max(double x, double y) {
    if (x > y) {
        return x;
    } else {
        return y;
    }
}
double min(double x, double y) {
    if (x < y) {
        return x;
    } else {
        return y;
    }
}
void Eliminare_Non_Maxime(fereastra * DET, int n) {
    unsigned int i, j;
    for (i = 0; i < n; i++) {
        if (DET[i].use == 1) {
            for (j = i + 1; j < n; j++) {
                double x1, y1, x2, y2, x3, y3, x4, y4, x5, y5, x6, y6, rez;
                x1 = DET[i].x;
                y1 = DET[i].y;
                x2 = DET[i].x + DET[i].inaltime;
                y2 = DET[i].y + DET[i].latime;
                x3 = DET[j].x;
                y3 = DET[j].y;
                x4 = DET[j].x + DET[j].inaltime;
                y4 = DET[j].y + DET[j].latime;
                x5 = max(x1, x3);
                y5 = max(y1, y3);
                x6 = min(x2, x4);
                y6 = min(y2, y4);
                if (x5 <= x6 && y5 <= y6) {
                    rez = 0;
                    rez = ((x6 - x5) * (y6 - y5)) / ((x2 - x2) * (y2 - y1) + (x4 - x3) * (y4 - y3) - (x6 - x5) * (y6 - y5));
                    if (rez >= 0.2) {
                        DET[j].use = 0;
                    }
                }
            }
        }
    }
}
int main() {
    unsigned int dim_img, latime_img, inaltime_img, padding, R0, SV;
    unsigned char * Header, col[10][3];
    unsigned int * R, * P, ** L, ** LP, ** C, ** D, nr, i, j, n;
    fereastra * F, * DET, * aux;
    char imagine_initiala[50], cheie_secreta[50], imagine_criptata[50], imagine_decriptata[50], sablon[50], imagine_finala[50];
    printf("Numele fisierului care continea imaginea: ");
    fgets(imagine_initiala, 50, stdin);
    imagine_initiala[strlen(imagine_initiala) - 1] = '\0';
    printf("Numele fisierului care contine cheia secreta: ");
    fgets(cheie_secreta, 50, stdin);
    cheie_secreta[strlen(cheie_secreta) - 1] = '\0';
    printf("Numele fisierului care contine imaginea criptata: ");
    fgets(imagine_criptata, 50, stdin);
    imagine_criptata[strlen(imagine_criptata) - 1] = '\0';
    printf("Numele fisierului care contine imaginea decriptata: ");
    fgets(imagine_decriptata, 50, stdin);
    imagine_decriptata[strlen(imagine_decriptata) - 1] = '\0';
    printf("Numele fisierului care contine imaginea finala : ");
    fgets(imagine_finala, 50, stdin);
    imagine_finala[strlen(imagine_finala) - 1] = '\0';
    Liniarizare(imagine_initiala, & dim_img, & latime_img, & inaltime_img, & padding, & L, & Header);
    printf("Dimensiunea imaginii in octeti: %u\n", dim_img);
    printf("Dimensiunea imaginii in pixeli (latime x inaltime): %u x %u\n", latime_img, inaltime_img);
    FILE * fin;
    fin = fopen(cheie_secreta, "r");
    fscanf(fin, "%u %u", & R0, & SV);
    fclose(fin);
    xorshift32(R0, 2 * latime_img * inaltime_img, & R);
    Permutare(latime_img * inaltime_img, & P, R);
    Criptare(cheie_secreta, L, & LP, & C, R, P, latime_img, inaltime_img);
    Decriptare(cheie_secreta, & D, C, P, R, latime_img, inaltime_img);
    Afisare(imagine_criptata, Header, C, latime_img, inaltime_img, padding);
    Afisare(imagine_decriptata, Header, D, latime_img, inaltime_img, padding);
    printf("Test pe imaginea initiala: \n");
    Test_Chi_Patrat(L, latime_img, inaltime_img);
    printf("Test pe imaginea criptata: \n");
    Test_Chi_Patrat(C, latime_img, inaltime_img);
    col[0][0] = 255;
    col[0][1] = 0;
    col[0][2] = 0;
    col[1][0] = 255;
    col[1][1] = 255;
    col[1][2] = 0;
    col[2][0] = 0;
    col[2][1] = 255;
    col[2][2] = 0;
    col[3][0] = 0;
    col[3][1] = 255;
    col[3][2] = 255;
    col[4][0] = 255;
    col[4][1] = 0;
    col[4][2] = 255;
    col[5][0] = 0;
    col[5][1] = 0;
    col[5][2] = 255;
    col[6][0] = 192;
    col[6][1] = 192;
    col[6][2] = 192;
    col[7][0] = 255;
    col[7][1] = 140;
    col[7][2] = 0;
    col[8][0] = 128;
    col[8][1] = 0;
    col[8][2] = 128;
    col[9][0] = 128;
    col[9][1] = 0;
    col[9][2] = 0;
    Imagine_Grayscale(imagine_initiala, L, latime_img, inaltime_img, padding);
    DET = (fereastra * ) malloc(sizeof(fereastra));
    n = 0;
    for (i = 0; i < 10; i++) {
        printf("Numele sablonului %u :", i);
        fgets(sablon, 50, stdin);
        sablon[strlen(sablon) - 1] = '\0';
        F = Detectii(sablon, L, latime_img, inaltime_img, padding, 0.5, i, & nr);
        aux = (fereastra * ) realloc(DET, (n + nr) * sizeof(fereastra));
        for (j = n; j < n + nr; j++) {
            aux[j] = F[j - n];
        }
        DET = aux;
        n = n + nr;
    }
    qsort(DET, n, sizeof(fereastra), cmp);
    Eliminare_Non_Maxime(DET, n);
    for (i = 0; i < n; i++) {
        if (DET[i].use == 1) {
            Colorare(L, col[DET[i].culoare], DET[i].x, DET[i].y, latime_img, inaltime_img, DET[i].latime, DET[i].inaltime);
        }
    }
    Afisare(imagine_finala, Header, L, latime_img, inaltime_img, padding);
    free(Header);
    free(R);
    free(P);
    free(F);
    free(DET);
    free(aux);
    for (i = 0; i < inaltime_img * latime_img; i++) {
        free(L[i]);
        free(LP[i]);
        free(C[i]);
        free(D[i]);
    }
    free(L);
    free(LP);
    free(C);
    free(D);
    return 0;
}
