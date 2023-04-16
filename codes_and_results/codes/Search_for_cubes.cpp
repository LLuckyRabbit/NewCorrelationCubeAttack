#include<iostream>
#include<fstream>
#include<stdio.h>
#include<string.h>
#include<vector>
#include<bitset>
#include<Windows.h>
#include "gurobi_c++.h"
using namespace std;
struct basis {
    char leadcoef[1000][80];
    int round;
    int feedbakpos;
    int size;
}coef;


int getdeg(char term[160])
{
    int deg = 0;
    for (int i = 0; i < 80; i++)
    {
        deg += term[i];
    }
    return deg;
}


/**********************************   Part One: polynomial operation     *******************************************/

int compare(char a[160], char b[160])
{
    int i;
    for (i = 0; i < 160; i++)
    {
        if (a[i] != b[i])
            return 0;
    }
    return 1;

}
int reduce(char** poly, int size)
{
    int i, j, k;
    char* flag, ** temp;
    flag = (char*)malloc(sizeof(char) * size);
    memset(flag, 0, sizeof(char) * size);
    temp = (char**)malloc(sizeof(char*) * size);
    for (i = 0; i < size; i++)
    {
        temp[i] = (char*)malloc(sizeof(char) * 160);
    }

    for (i = 0; i < size - 1; i++)
    {
        if (flag[i] == 0)
        {
            for (j = i + 1; j < size; j++)
            {
                if (flag[j] == 0)
                {
                    if (compare(poly[i], poly[j]))
                    {
                        flag[i] = 1;
                        flag[j] = 1;
                        break;
                    }
                }
            }
        }
    }
    k = 0;
    for (i = 0; i < size; i++)
    {
        if (flag[i] == 0)
        {
            for (j = 0; j < 160; j++)
            {
                temp[k][j] = poly[i][j];
            }
            k++;
        }
    }
    for (i = 0; i < size; i++)
    {
        memset(poly[i], 0, sizeof(char) * 160);
    }
    for (i = 0; i < k; i++)
    {
        for (j = 0; j < 160; j++)
        {
            poly[i][j] = temp[i][j];
        }
    }
    for (i = 0; i < size; i++)
    {
        free(temp[i]);
    }
    free(temp);
    free(flag);
    return k;
}

int multi(char** s1, char** s2, int size1, int size2, char** res)
{
    int i, j, k;
    int size = size1 * size2;
    char** temp;
    temp = (char**)malloc(sizeof(char*) * size);
    for (i = 0; i < size; i++)
    {
        temp[i] = (char*)malloc(sizeof(char) * 160);
    }
    for (i = 0; i < size1; i++)
    {
        for (j = 0; j < size2; j++)
        {
            for (k = 0; k < 160; k++)
            {
                temp[i * size2 + j][k] = max(s1[i][k], s2[j][k]);
            }
        }
    }


    size = reduce(temp, size1 * size2);
    for (i = 0; i < size; i++)
    {
        for (j = 0; j < 160; j++)
        {
            res[i][j] = temp[i][j];
        }
    }
    for (i = 0; i < size1 * size2; i++)
    {
        free(temp[i]);
    }
    free(temp);
    return size;
}

int addition(char** s1, char** s2, char** s3, char** s4, int size1, int size2, int size3, int size4, char** res)
{

    int i, j;
    int size = size1 + size2 + size3 + size4;
    char** temp;
    temp = (char**)malloc(sizeof(char*) * size);

    for (i = 0; i < size; i++)
    {
        temp[i] = (char*)malloc(sizeof(char) * 160);
    }
    for (i = 0; i < size1; i++)
    {
        for (j = 0; j < 160; j++)
        {
            temp[i][j] = s1[i][j];
        }
    }
    for (i = 0; i < size2; i++)
    {
        for (j = 0; j < 160; j++)
        {
            temp[i + size1][j] = s2[i][j];
            //printf("%d", s2[i][j]);
        }

    }
    for (i = 0; i < size3; i++)
    {
        for (j = 0; j < 160; j++)
        {
            temp[i + size1 + size2][j] = s3[i][j];
        }
    }
    for (i = 0; i < size4; i++)
    {
        for (j = 0; j < 160; j++)
        {
            temp[i + size1 + size2 + size3][j] = s4[i][j];
        }
    }

    size = reduce(temp, size);
    for (i = 0; i < size; i++)
    {
        for (j = 0; j < 160; j++)
        {
            res[i][j] = temp[i][j];
        }
    }

    for (i = 0; i < size1 + size2 + size3 + size4; i++)
    {
        free(temp[i]);
    }
    free(temp);
    return size;
}


/***********************************    Part Two:  finding the basis    ******************************************/

int getleadcoef(char** poly, int size, char coef[][80], int DEG[2], int vanpos[], int vansize[])
{
    int i, j, k;
    int deg;
    int maxdeg = -10000;
    int submaxdeg = -10000;
    char* termdeg, * flag;
    termdeg = (char*)malloc(sizeof(char) * size);
    flag = (char*)malloc(sizeof(char) * size);
    memset(flag, 0, sizeof(char) * size);


    for (i = 0; i < size; i++)
    {
        termdeg[i] = getdeg(poly[i]);
        if (termdeg[i] > maxdeg)
        {
            maxdeg = termdeg[i];
        }
    }

    if (maxdeg == 0)
    {
        DEG[0] = 0;
        DEG[1] = 0;
        vansize[0] = 0;
        return 0;
    }


    for (i = 0; i < size; i++)
    {
        deg = termdeg[i];
        if ((deg > submaxdeg) && (deg != maxdeg))
        {
            submaxdeg = deg;
        }
    }

    int m = 0;
    for (i = 0; i < size; i++)
    {
        if (flag[i] == 0)
        {
            if (termdeg[i] == maxdeg)
            {
                for (k = 0; k < 80; k++)
                {
                    coef[m][k] = poly[i][80 + k];
                }
                vanpos[m] = i;
                m++;
                for (j = i + 1; j < size; j++)
                {
                    if (termdeg[j] == maxdeg)
                    {
                        for (k = 0; k < 80; k++)
                        {
                            if (poly[j][k] != poly[i][k])
                                break;
                        }
                        
                        if (k == 80)
                        {
                            flag[j] = 1;
                            for (k = 0; k < 80; k++)
                            {
                                coef[m][k] = poly[j][k + 80];
                            }
                            vanpos[m] = j;
                            m++;
                        }
                    }
                }
                for (k = 0; k < 80; k++)
                {
                    coef[m][k] = 1;
                }
            }
        }

    }

    vansize[0] = m;
    DEG[0] = maxdeg;
    DEG[1] = submaxdeg;

    free(termdeg);
    free(flag);
    return m;
}

int vanish(char** poly, int vanpos[], int size, int vansize)
{
    int i, j;
    char* flag;
    char** temp;
    temp = (char**)malloc(sizeof(char*) * size);
    flag = (char*)malloc(sizeof(char) * size);
    memset(flag, 0, sizeof(char) * size);
    for (i = 0; i < size; i++)
        temp[i] = (char*)malloc(sizeof(char) * 160);
    for (i = 0; i < vansize; i++)
        flag[vanpos[i]] = 1;
    int m = 0;
    for (i = 0; i < size; i++)
    {
        if (flag[i] == 0)
        {
            for (j = 0; j < 160; j++)
            {
                temp[m][j] = poly[i][j];
            }
            m++;
        }
    }
    for (i = 0; i < size; i++)
    {
        memset(poly[i], 0, sizeof(char) * 160);
    }
    for (i = 0; i < m; i++)
    {
        for (j = 0; j < 160; j++)
        {
            poly[i][j] = temp[i][j];
        }
    }
    for (i = 0; i < size; i++)
    {
        free(temp[i]);
    }
    free(temp);
    free(flag);
    return m;
}

void Decomposition(int cube[], int dim, int N0, char*** s, int vfA[], int vfB[], int vfC[], int size[])
{
    int i, j;
    char** f1, ** f2, ** f3, ** t1, ** t2, ** t3;
    f1 = (char**)malloc(sizeof(char*) * 10000);
    f2 = (char**)malloc(sizeof(char*) * 10000);
    f3 = (char**)malloc(sizeof(char*) * 10000);
    t1 = (char**)malloc(sizeof(char*) * 10000);
    t2 = (char**)malloc(sizeof(char*) * 10000);
    t3 = (char**)malloc(sizeof(char*) * 10000);
    for (i = 0; i < 10000; i++)
    {
        f1[i] = (char*)malloc(sizeof(char) * 160);
        f2[i] = (char*)malloc(sizeof(char) * 160);
        f3[i] = (char*)malloc(sizeof(char) * 160);
        t1[i] = (char*)malloc(sizeof(char) * 160);
        t2[i] = (char*)malloc(sizeof(char) * 160);
        t3[i] = (char*)malloc(sizeof(char) * 160);
        memset(f1[i], 0, sizeof(char) * 160);
        memset(f2[i], 0, sizeof(char) * 160);
        memset(f3[i], 0, sizeof(char) * 160);
        memset(t1[i], 0, sizeof(char) * 160);
        memset(t2[i], 0, sizeof(char) * 160);
        memset(t3[i], 0, sizeof(char) * 160);
    }


    //Initialize
    for (i = 0; i < 80; i++)
    {
        s[i][0][80 + i] = 1;
        size[i] = 1;
    }
    for (i = 0; i < dim; i++)
    {
        s[93 + cube[i]][0][cube[i]] = 1;
        size[93 + cube[i]] = 1;
    }
    for (i = 285; i < 288; i++)
    {
        size[i] = 1;
    }




    int k, l, m = 0, tempdeg = 0;
    FILE* fp;
    fopen_s(&fp, "Res.txt", "a+");
    int round = N0;
    int vanpos1[100], vanpos2[100], vanpos3[100];
    int vansizet1[1], vansizet2[1], vansizet3[1];
    int size1, size2, size3, flag1, flag2, flag3;
    int DEG1[2], DEG2[2], DEG3[2]; 
    int degt1, degt2, degt3;
    int coefnum1, coefnum2, coefnum3;
    char coeft1[1000][80] = { 0 }, coeft2[1000][80] = { 0 }, coeft3[1000][80] = { 0 };
    basis* leadcoefs;
    leadcoefs = (basis*)malloc(sizeof(basis) * 600);



    for (i = 0; i < round; i++)
    {
        //s66+s91*s92+s93+s171;
        size1 = multi(s[91 - 1], s[92 - 1], size[91 - 1], size[92 - 1], f1);
        size1 = addition(f1, s[66 - 1], s[93 - 1], s[171 - 1], size1, size[66 - 1], size[93 - 1], size[171 - 1], t1);
        //s162+s175*s176+size177+s264
        size2 = multi(s[175 - 1], s[176 - 1], size[175 - 1], size[176 - 1], f2);
        size2 = addition(f2, s[162 - 1], s[177 - 1], s[264 - 1], size2, size[162 - 1], size[177 - 1], size[264 - 1], t2);
        //s243+s286*s287+size288+s69
        size3 = multi(s[286 - 1], s[287 - 1], size[286 - 1], size[287 - 1], f3);
        size3 = addition(f3, s[243 - 1], s[288 - 1], s[69 - 1], size3, size[243 - 1], size[288 - 1], size[69 - 1], t3);


        memset(vanpos1, 0, sizeof(int) * 1000);
        memset(vanpos2, 0, sizeof(int) * 1000);
        memset(vanpos3, 0, sizeof(int) * 1000);
        for (j = 0; j < 1000; j++)
        {
            memset(coeft1[i], 0, sizeof(char) * 80);
            memset(coeft2[i], 0, sizeof(char) * 80);
            memset(coeft3[i], 0, sizeof(char) * 80);
        }

        coefnum1 = getleadcoef(t1, size1, coeft1, DEG1, vanpos1, vansizet1);
        coefnum2 = getleadcoef(t2, size2, coeft2, DEG2, vanpos2, vansizet2);
        coefnum3 = getleadcoef(t3, size3, coeft3, DEG3, vanpos3, vansizet3);

        flag1 = 0;
        flag2 = 0;
        flag3 = 0;
        if (coefnum1 == 0)
            flag1 = 1;
        if (coefnum2 == 0)
            flag2 = 1;
        if (coefnum3 == 0)
            flag3 = 1;

        for (k = 0; k < coefnum1; k++)
        {
            tempdeg = 0;
            for (j = 0; j < 80; j++)
            {
                tempdeg += coeft1[k][j];
            }
            if (tempdeg == 0)
            {
                flag1 = 1;
                break;
            }
        }
        for (k = 0; k < coefnum2; k++)
        {
            tempdeg = 0;
            for (j = 0; j < 80; j++)
            {
                tempdeg += coeft2[k][j];
            }
            if (tempdeg == 0)
            {
                flag2 = 1;
                break;
            }
        }

        for (k = 0; k < coefnum3; k++)
        {
            tempdeg = 0;
            for (j = 0; j < 80; j++)
            {
                tempdeg += coeft3[k][j];
            }
            if (tempdeg == 0)
            {
                flag3 = 1;
                break;
            }
        }

        if (flag1 == 0)
        {
            leadcoefs[m].round = i;
            leadcoefs[m].feedbakpos = 1;
            leadcoefs[m].size = coefnum1;
            for (j = 0; j < coefnum1; j++)
            {
                for (k = 0; k < 80; k++)
                {
                    leadcoefs[m].leadcoef[j][k] = coeft1[j][k];
                }

            }
            m++;
            degt1 = DEG1[1];
            size1 = vanish(t1, vanpos1, size1, vansizet1[0]);
            vfB[i] = 1;
        }
        else
        {
            degt1 = DEG1[0];
            vfB[i] = 0;
        }
        if (flag2 == 0)
        {
            leadcoefs[m].round = i;
            leadcoefs[m].feedbakpos = 2;
            leadcoefs[m].size = coefnum2;
            for (j = 0; j < coefnum2; j++)
            {
                for (k = 0; k < 80; k++)
                {
                    leadcoefs[m].leadcoef[j][k] = coeft2[j][k];
                }

            }
            m++;
            degt2 = DEG2[1];
            size2 = vanish(t2, vanpos2, size2, vansizet2[0]);
            vfC[i] = 1;
        }
        else
        {
            degt2 = DEG2[0];
            vfC[i] = 0;
        }

        if (flag3 == 0)
        {
            leadcoefs[m].round = i;
            leadcoefs[m].feedbakpos = 3;
            leadcoefs[m].size = coefnum3;
            for (j = 0; j < coefnum3; j++)
            {
                for (k = 0; k < 80; k++)
                {
                    leadcoefs[m].leadcoef[j][k] = coeft3[j][k];
                }
            }
            m++;
            degt3 = DEG3[1];
            size3 = vanish(t3, vanpos3, size3, vansizet3[0]);
            vfA[i] = 1;
        }
        else
        {
            degt3 = DEG3[0];
            vfA[i] = 0;
        }



        for (j = 92; j > 0; j--)
        {
            for (k = 0; k < size[j]; k++)
                memset(s[j][k], 0, sizeof(char) * 160);
            for (k = 0; k < size[j - 1]; k++)
            {
                for (l = 0; l < 160; l++)
                {
                    s[j][k][l] = s[j - 1][k][l];
                }
            }
            size[j] = size[j - 1];
        }
        for (j = 176; j > 93; j--)
        {
            for (k = 0; k < size[j]; k++)
                memset(s[j][k], 0, sizeof(char) * 160);
            for (k = 0; k < size[j - 1]; k++)
            {
                for (l = 0; l < 160; l++)
                {
                    s[j][k][l] = s[j - 1][k][l];
                }
            }
            size[j] = size[j - 1];
        }
        for (j = 287; j > 177; j--)
        {
            for (k = 0; k < size[j]; k++)
                memset(s[j][k], 0, sizeof(char) * 160);
            for (k = 0; k < size[j - 1]; k++)
            {
                for (l = 0; l < 160; l++)
                {
                    s[j][k][l] = s[j - 1][k][l];
                }
            }
            size[j] = size[j - 1];
        }
        //s0
        for (k = 0; k < size[0]; k++)
            memset(s[0][k], 0, sizeof(char) * 160);
        for (j = 0; j < size3; j++)
        {
            for (k = 0; k < 160; k++)
            {
                s[0][j][k] = t3[j][k];
            }
        }
        size[0] = size3;
        //s93
        for (k = 0; k < size[93]; k++)
            memset(s[93][k], 0, sizeof(char) * 160);
        for (j = 0; j < size1; j++)
        {
            for (k = 0; k < 160; k++)
            {
                s[93][j][k] = t1[j][k];
            }
        }
        size[93] = size1;
        //s177
        for (k = 0; k < size[177]; k++)
            memset(s[177][k], 0, sizeof(char) * 160);
        for (j = 0; j < size2; j++)
        {
            for (k = 0; k < 160; k++)
            {
                s[177][j][k] = t2[j][k];
            }
        }
        size[177] = size2;

    for (i = 0; i < m; i++)
    {
        fprintf(fp, "the %d th round leadcoefs of Reg %d\n", leadcoefs[i].round, leadcoefs[i].feedbakpos);
        for (k = 0; k < leadcoefs[i].size; k++)
        {
            for (j = 0; j < 80; j++)
            {
                if (leadcoefs[i].leadcoef[k][j])
                    fprintf(fp, "k(%d)", j);
            }
            if (leadcoefs[i].size > 1)
                fprintf(fp, "+");
        }
        fprintf(fp, "\n");
    }
    for (i = 0; i < 10000; i++)
    {
        free(f1[i]);
        free(f2[i]);
        free(f3[i]);
        free(t1[i]);
        free(t2[i]);
        free(t3[i]);
    }
    free(f1);
    free(f2);
    free(f3);
    free(t1);
    free(t2);
    free(t3);

    free(leadcoefs);
    fclose(fp);
}

void Decomposition_V2(int cube[], int dim, int N0, char*** s, int minvfA[], int minvfB[], int minvfC[], int size[])
{
    int i, j;
    char** f1, ** f2, ** f3, ** t1, ** t2, ** t3;
    f1 = (char**)malloc(sizeof(char*) * 10000);
    f2 = (char**)malloc(sizeof(char*) * 10000);
    f3 = (char**)malloc(sizeof(char*) * 10000);
    t1 = (char**)malloc(sizeof(char*) * 10000);
    t2 = (char**)malloc(sizeof(char*) * 10000);
    t3 = (char**)malloc(sizeof(char*) * 10000);
    for (i = 0; i < 10000; i++)
    {
        f1[i] = (char*)malloc(sizeof(char) * 160);
        f2[i] = (char*)malloc(sizeof(char) * 160);
        f3[i] = (char*)malloc(sizeof(char) * 160);
        t1[i] = (char*)malloc(sizeof(char) * 160);
        t2[i] = (char*)malloc(sizeof(char) * 160);
        t3[i] = (char*)malloc(sizeof(char) * 160);
        memset(f1[i], 0, sizeof(char) * 160);
        memset(f2[i], 0, sizeof(char) * 160);
        memset(f3[i], 0, sizeof(char) * 160);
        memset(t1[i], 0, sizeof(char) * 160);
        memset(t2[i], 0, sizeof(char) * 160);
        memset(t3[i], 0, sizeof(char) * 160);
    }

    //Initialize
    for (i = 0; i < 80; i++)
    {
        s[i][0][80 + i] = 1;
        size[i] = 1;
    }

    for (i = 0; i < dim; i++)
    {
        s[93 + cube[i]][0][cube[i]] = 1;
        size[93 + cube[i]] = 1;
    }
    for (i = 285; i < 288; i++)
    {
        size[i] = 1;
    }



    int k, l, m = 0, tempdeg = 0;
    int round = N0;
    int vanpos1[100], vanpos2[100], vanpos3[100];
    int vansizet1[1], vansizet2[1], vansizet3[1];
    int size1, size2, size3, flag1, flag2, flag3;
    int DEG1[2], DEG2[2], DEG3[2];
    int coefnum1, coefnum2, coefnum3;
    char coeft1[1000][80] = { 0 }, coeft2[1000][80] = { 0 }, coeft3[1000][80] = { 0 };
    basis* leadcoefs;
    leadcoefs = (basis*)malloc(sizeof(basis) * 600);



    for (i = 0; i < round; i++)
    {


        //s66+s91*s92+s93+s171;
        size1 = multi(s[91 - 1], s[92 - 1], size[91 - 1], size[92 - 1], f1);
        size1 = addition(f1, s[66 - 1], s[93 - 1], s[171 - 1], size1, size[66 - 1], size[93 - 1], size[171 - 1], t1);
        //s162+s175*s176+size177+s264
        size2 = multi(s[175 - 1], s[176 - 1], size[175 - 1], size[176 - 1], f2);
        size2 = addition(f2, s[162 - 1], s[177 - 1], s[264 - 1], size2, size[162 - 1], size[177 - 1], size[264 - 1], t2);
        //s243+s286*s287+size288+s69
        size3 = multi(s[286 - 1], s[287 - 1], size[286 - 1], size[287 - 1], f3);
        size3 = addition(f3, s[243 - 1], s[288 - 1], s[69 - 1], size3, size[243 - 1], size[288 - 1], size[69 - 1], t3);



        memset(vanpos1, 0, sizeof(int) * 1000);
        memset(vanpos2, 0, sizeof(int) * 1000);
        memset(vanpos3, 0, sizeof(int) * 1000);
        for (j = 0; j < 1000; j++)
        {
            memset(coeft1[i], 0, sizeof(char) * 80);
            memset(coeft2[i], 0, sizeof(char) * 80);
            memset(coeft3[i], 0, sizeof(char) * 80);
        }


        coefnum1 = getleadcoef(t1, size1, coeft1, DEG1, vanpos1, vansizet1);
        coefnum2 = getleadcoef(t2, size2, coeft2, DEG2, vanpos2, vansizet2);
        coefnum3 = getleadcoef(t3, size3, coeft3, DEG3, vanpos3, vansizet3);


        flag1 = 0;
        flag2 = 0;
        flag3 = 0;
        if (coefnum1 == 0)
            flag1 = 1;
        if (coefnum2 == 0)
            flag2 = 1;
        if (coefnum3 == 0)
            flag3 = 1;

        for (k = 0; k < coefnum1; k++)
        {
            tempdeg = 0;
            for (j = 0; j < 80; j++)
            {
                tempdeg += coeft1[k][j];
            }
            if (tempdeg == 0)
            {
                flag1 = 1;
                break;
            }
        }
        for (k = 0; k < coefnum2; k++)
        {
            tempdeg = 0;
            for (j = 0; j < 80; j++)
            {
                tempdeg += coeft2[k][j];
            }
            if (tempdeg == 0)
            {
                flag2 = 1;
                break;
            }
        }

        for (k = 0; k < coefnum3; k++)
        {
            tempdeg = 0;
            for (j = 0; j < 80; j++)
            {
                tempdeg += coeft3[k][j];
            }
            if (tempdeg == 0)
            {
                flag3 = 1;
                break;
            }
        }


        if (flag1 == 0 && minvfB[i] == 1)
        {
            leadcoefs[m].round = i;
            leadcoefs[m].feedbakpos = 1;
            leadcoefs[m].size = coefnum1;
            for (j = 0; j < coefnum1; j++)
            {
                for (k = 0; k < 80; k++)
                {
                    leadcoefs[m].leadcoef[j][k] = coeft1[j][k];
                }

            }
            m++;
            size1 = vanish(t1, vanpos1, size1, vansizet1[0]);
        }

        if (flag2 == 0 && minvfC[i] == 1)
        {
            leadcoefs[m].round = i;
            leadcoefs[m].feedbakpos = 2;
            leadcoefs[m].size = coefnum2;
            for (j = 0; j < coefnum2; j++)
            {
                for (k = 0; k < 80; k++)
                {
                    leadcoefs[m].leadcoef[j][k] = coeft2[j][k];
                }

            }
            m++;
            size2 = vanish(t2, vanpos2, size2, vansizet2[0]);
        }

        if (flag3 == 0 && minvfA[i] == 1)
        {
            leadcoefs[m].round = i;
            leadcoefs[m].feedbakpos = 3;
            leadcoefs[m].size = coefnum3;
            for (j = 0; j < coefnum3; j++)
            {
                for (k = 0; k < 80; k++)
                {
                    leadcoefs[m].leadcoef[j][k] = coeft3[j][k];
                }
            }
            m++;
            size3 = vanish(t3, vanpos3, size3, vansizet3[0]);
        }

        for (j = 92; j > 0; j--)
        {
            for (k = 0; k < size[j]; k++)
                memset(s[j][k], 0, sizeof(char) * 160);
            for (k = 0; k < size[j - 1]; k++)
            {
                for (l = 0; l < 160; l++)
                {
                    s[j][k][l] = s[j - 1][k][l];
                }
            }
            size[j] = size[j - 1];
        }
        for (j = 176; j > 93; j--)
        {
            for (k = 0; k < size[j]; k++)
                memset(s[j][k], 0, sizeof(char) * 160);
            for (k = 0; k < size[j - 1]; k++)
            {
                for (l = 0; l < 160; l++)
                {
                    s[j][k][l] = s[j - 1][k][l];
                }
            }
            size[j] = size[j - 1];
        }
        for (j = 287; j > 177; j--)
        {

            for (k = 0; k < size[j]; k++)
                memset(s[j][k], 0, sizeof(char) * 160);
            for (k = 0; k < size[j - 1]; k++)
            {
                for (l = 0; l < 160; l++)
                {
                    s[j][k][l] = s[j - 1][k][l];
                }
            }
            size[j] = size[j - 1];
        }

        //s0
        for (k = 0; k < size[0]; k++)
            memset(s[0][k], 0, sizeof(char) * 160);
        for (j = 0; j < size3; j++)
        {
            for (k = 0; k < 160; k++)
            {
                s[0][j][k] = t3[j][k];
            }
        }
        size[0] = size3;
        //s93
        for (k = 0; k < size[93]; k++)
            memset(s[93][k], 0, sizeof(char) * 160);
        for (j = 0; j < size1; j++)
        {
            for (k = 0; k < 160; k++)
            {
                s[93][j][k] = t1[j][k];
            }
        }
        size[93] = size1;
        //s177
        for (k = 0; k < size[177]; k++)
            memset(s[177][k], 0, sizeof(char) * 160);
        for (j = 0; j < size2; j++)
        {
            for (k = 0; k < 160; k++)
            {
                s[177][j][k] = t2[j][k];
            }
        }
        size[177] = size2;

    }

    for (i = 0; i < 10000; i++)
    {
        free(f1[i]);
        free(f2[i]);
        free(f3[i]);
        free(t1[i]);
        free(t2[i]);
        free(t3[i]);
    }
    free(f1);
    free(f2);
    free(f3);
    free(t1);
    free(t2);
    free(t3);
    free(leadcoefs);
}


/********************************   Part Three: evaluate algebraic degrees     *************************************/

void triviumCore(GRBModel& model, vector<GRBVar>& x, int i1, int i2, int i3, int i4, int i5)
{
    GRBVar y1 = model.addVar(0, 1, 0, GRB_BINARY);
    GRBVar y2 = model.addVar(0, 1, 0, GRB_BINARY);
    GRBVar y3 = model.addVar(0, 1, 0, GRB_BINARY);
    GRBVar y4 = model.addVar(0, 1, 0, GRB_BINARY);
    GRBVar y5 = model.addVar(0, 1, 0, GRB_BINARY);

    GRBVar z1 = model.addVar(0, 1, 0, GRB_BINARY);
    GRBVar z2 = model.addVar(0, 1, 0, GRB_BINARY);

    // z3 and z4 are not needed, since z3 = z4 = a
    GRBVar a = model.addVar(0, 1, 0, GRB_BINARY);

    //copy
    model.addConstr(y1 <= x[i1]);
    model.addConstr(z1 <= x[i1]);
    model.addConstr(y1 + z1 >= x[i1]);

    //copy
    model.addConstr(y2 <= x[i2]);
    model.addConstr(z2 <= x[i2]);
    model.addConstr(y2 + z2 >= x[i2]);

    //copy
    model.addConstr(y3 <= x[i3]);
    model.addConstr(a <= x[i3]);
    model.addConstr(y3 + a >= x[i3]);

    //copy
    model.addConstr(y4 <= x[i4]);
    model.addConstr(a <= x[i4]);
    model.addConstr(y4 + a >= x[i4]);
    //XOR
    model.addConstr(y5 == x[i5] + a + z1 + z2);

    x[i1] = y1;
    x[i2] = y2;
    x[i3] = y3;
    x[i4] = y4;
    x[i5] = y5;
}

//char s[288][10000][160] = {0};
int computeDPDegree(int cube[], int dim, char*** s, int size[], int Round, int N0)
{
    int deg = 0;
    int i, j, l;
    int count[160] = { 0 };
    for (i = 0; i < 288; i++)
    {
        for (j = 0; j < size[i]; j++)
        {
            for (l = 0; l < 160; l++)
            {
                if (s[i][j][l] == 1)
                {
                    count[l]++;
                }
            }
        }
    }


    GRBEnv* env = 0;
    env = new GRBEnv;


    GRBModel model = GRBModel(*env);

    model.set(GRB_IntParam_LogToConsole, 0);
    model.set(GRB_IntParam_PoolSearchMode, 1);  
    model.set(GRB_IntParam_PoolSolutions, 1);
    //env.set(GRB_IntParam_Threads, 8);

    vector<GRBVar> works(288);
    for (i = 0; i < 288; i++)
        works[i] = model.addVar(0, 1, 0, GRB_BINARY);

    //
    vector<GRBVar>  s_anf(161 * 300);
    for (i = 0; i < 160; i++)
    {
        for (j = 0; j < count[i]; j++)
        {
            s_anf[300 * i + j] = model.addVar(0, 1, 0, GRB_BINARY);
        }
    }
    for (i = 0; i < 288; i++)
        s_anf[300 * 160 + i] = model.addVar(0, 1, 0, GRB_BINARY);


    // copy KEY
    for (i = 0; i < 80; i++)
    {
        GRBLinExpr tttteee = 0;
        for (j = 0; j < count[i + 80]; j++)
        {
            model.addConstr(s_anf[(i + 80) * 300 + j] <= works[i]);
            tttteee += s_anf[(i + 80) * 300 + j];
        }
        model.addConstr(tttteee >= works[i]);
    }
    // copy IV
    for (i = 0; i < 80; i++)
    {
        GRBLinExpr tttteee = 0;
        for (j = 0; j < count[i]; j++)
        {
            model.addConstr(s_anf[i * 300 + j] <= works[i + 93]);
            tttteee += s_anf[i * 300 + j];
        }
        model.addConstr(tttteee >= works[i + 93]);
    }

    vector<GRBVar> works_new(288); 
    for (i = 0; i < 288; i++)
        works_new[i] = model.addVar(0, 1, 0, GRB_BINARY);
    int count_loc[161] = { 0 }; 

    int flafg;
    for (i = 0; i < 288; i++)
    {
        if (size[i] == 0)
            model.addConstr(works_new[i] == 0);
        else
        {
            GRBLinExpr nk_temp = 0;

            for (j = 0; j < size[i]; j++)
            {
                GRBVar temp_var = model.addVar(0, 1, 0, GRB_BINARY);
                flafg = 0;
                for (l = 0; l < 160; l++)
                {
                    if (s[i][j][l] == 1)
                    {
                        model.addConstr(temp_var == s_anf[l * 300 + count_loc[l]]);
                        count_loc[l]++;
                        flafg = 1;
                    }
                }
                if (flafg == 0)
                {
                    model.addConstr(temp_var == s_anf[160 * 300 + count_loc[160]]);
                    count_loc[160]++;
                }
                nk_temp += temp_var;
            }
            model.addConstr(works_new[i] == nk_temp);
        }
    }

    /*
    for (i = 0; i < 161; i++)
    {
        printf("%d\n", count_loc[i]);
    }*/


    for (int r = 0; r < Round - N0; r++)
    {
        triviumCore(model, works_new, 65, 170, 90, 91, 92);
        triviumCore(model, works_new, 161, 263, 174, 175, 176);
        triviumCore(model, works_new, 242, 68, 285, 286, 287);

        vector<GRBVar> temp = works_new;
        for (i = 0; i < 288; i++)
            works_new[(i + 1) % 288] = temp[i];
    }


    // output constraints
    GRBLinExpr nk = 0;
    for (int i = 0; i < 288; i++)
        if ((i == 65) || (i == 92) || (i == 161) || (i == 176) || (i == 242) || (i == 287))
            nk += works_new[i];
        else
            model.addConstr(works_new[i] == 0);
    model.addConstr(nk == 1);


    GRBLinExpr key_sum = 0;
    for (i = 0; i < 80; i++)
        key_sum += works[i];
    model.setObjective(key_sum, GRB_MAXIMIZE);

    bitset<80> cube_term(0);
    for (i = 0; i < dim; i++)
        cube_term.set(cube[i]);

    for (i = 0; i < 80; i++)
    {
        if (cube_term.test(i))
            model.addConstr(works[93 + i] == 1);
        else
            model.addConstr(works[93 + i] == 0);
    }


    model.update();
    model.optimize();



    if (model.get(GRB_IntAttr_Status) == GRB_OPTIMAL)
    {
        delete env;
        return 1;
    }
    else
    {
        delete env;
        return -1;
    }

}




void minimize_basis(int cube[], int dim, int Round, int vfA[], int vfB[], int vfC[], int N0, int size[])
{
    int i, j, k;
    int minvfA[300] = { 0 }, minvfB[300] = { 0 }, minvfC[300] = { 0 };
    int findeg, bsnum;
    for (i = 0; i < N0; i++)
    {
        minvfA[i] = vfA[i];
        minvfB[i] = vfB[i];
        minvfC[i] = vfC[i];
    }
    for (i = N0; i >= 0; i--)
    {
        //printf("%d, ", i);
        if (vfA[i] == 1)
        {
           
            minvfA[i] = 0;
            char*** s;

            s = (char***)malloc(sizeof(char**) * 288);
            for (k = 0; k < 288; k++)
            {
                s[k] = (char**)malloc(sizeof(char*) * 10000);
                for (j = 0; j < 10000; j++)
                {
                    s[k][j] = (char*)malloc(sizeof(char) * 160);
                    memset(s[k][j], 0, sizeof(char) * 160);
                }
            }
            for (k = 0; k < 288; k++)
            {
                size[k] = 0;
            }
            Decomposition_V2(cube, dim, N0, s, minvfA, minvfB, minvfC, size);
            findeg = computeDPDegree(cube, dim, s, size, Round, N0);
            if (findeg == 1)
            {
                minvfA[i] = 1;
            }
            for (j = 0; j < 288; j++)
            {
                for (k = 0; k < 10000; k++)
                {
                    free(s[j][k]);
                }
                free(s[j]);
            }
            free(s);
            printf("%d %d \n", i, findeg);
        }
        if (vfB[i] == 1)
        {
            //char s[288][10000][160] = {0};  
            char*** s;

            s = (char***)malloc(sizeof(char**) * 288);
            for (k = 0; k < 288; k++)
            {
                s[k] = (char**)malloc(sizeof(char*) * 10000);
                for (j = 0; j < 10000; j++)
                {
                    s[k][j] = (char*)malloc(sizeof(char) * 160);
                    memset(s[k][j], 0, sizeof(char) * 160);
                }
            }
            for (k = 0; k < 288; k++)
            {
                size[k] = 0;
            }
            minvfB[i] = 0;
            Decomposition_V2(cube, dim, N0, s, minvfA, minvfB, minvfC, size);
            findeg = computeDPDegree(cube, dim, s, size, Round, N0);
            if (findeg == 1)
            {
                minvfB[i] = 1;
            }
            for (j = 0; j < 288; j++)
            {
                for (k = 0; k < 10000; k++)
                {
                    free(s[j][k]);
                }
                free(s[j]);
            }
            free(s);
            printf("%d %d \n", i, findeg);
        }
        if (vfC[i] == 1)
        {
            //char s[288][10000][160] = {0}; 
            char*** s;

            s = (char***)malloc(sizeof(char**) * 288);
            for (k = 0; k < 288; k++)
            {
                s[k] = (char**)malloc(sizeof(char*) * 10000);
                for (j = 0; j < 10000; j++)
                {
                    s[k][j] = (char*)malloc(sizeof(char) * 160);
                    memset(s[k][j], 0, sizeof(char) * 160);
                }
            }
            for (k = 0; k < 288; k++)
            {
                size[k] = 0;
            }
            minvfC[i] = 0;
            Decomposition_V2(cube, dim, N0, s, minvfA, minvfB, minvfC, size);
            findeg = computeDPDegree(cube, dim, s, size, Round, N0);
            if (findeg == 1)
            {
                minvfC[i] = 1;
            }
            for (j = 0; j < 288; j++)
            {
                for (k = 0; k < 10000; k++)
                {
                    free(s[j][k]);
                }
                free(s[j]);
            }
            free(s);
            printf("%d %d \n", i, findeg);
        }
        //printf("\n");
    }
    printf("\n");
    bsnum = 0;
    for (i = 0; i < N0; i++)
    {
        if ((minvfA[i] == 1) || (minvfB[i] == 1) || (minvfC[i] == 1))
        {
            bsnum++;
        }
    }
    FILE* fpout;
    fopen_s(&fpout, "Res.txt", "a+");
    fprintf(fpout, "\n%d basis are needed: ", bsnum);
    for (i = 0; i < N0; i++)
    {
        if ((minvfA[i] == 1) || (minvfB[i] == 1) || (minvfC[i] == 1))
        {
            fprintf(fpout, "%d ", i);
        }
    }
    fprintf(fpout, "\n");
    fclose(fpout);


    bsnum = 0;
    for (i = 0; i < N0; i++)
    {
        minvfA[i] = vfA[i];
        minvfB[i] = vfB[i];
        minvfC[i] = vfC[i];
    }
    for (i = 0; i <= N0; i++)
    {
        //printf("%d, ", i);
        if (vfA[i] == 1)
        {
            char*** s;

            s = (char***)malloc(sizeof(char**) * 288);
            for (k = 0; k < 288; k++)
            {
                s[k] = (char**)malloc(sizeof(char*) * 10000);
                for (j = 0; j < 10000; j++)
                {
                    s[k][j] = (char*)malloc(sizeof(char) * 160);
                    memset(s[k][j], 0, sizeof(char) * 160);
                }
            }
            for (k = 0; k < 288; k++)
            {
                size[k] = 0;
            }
            minvfA[i] = 0;
            Decomposition_V2(cube, dim, N0, s, minvfA, minvfB, minvfC, size);
            findeg = computeDPDegree(cube, dim, s, size, Round, N0);
            if (findeg == 1)
            {
                minvfA[i] = 1;
            }
            for (j = 0; j < 288; j++)
            {
                for (k = 0; k < 10000; k++)
                {
                    free(s[j][k]);
                }
                free(s[j]);
            }
            free(s);
            printf("%d %d \n", i, findeg);
        }
        if (vfB[i] == 1)
        {
            char*** s;

            s = (char***)malloc(sizeof(char**) * 288);
            for (k = 0; k < 288; k++)
            {
                s[k] = (char**)malloc(sizeof(char*) * 10000);
                for (j = 0; j < 10000; j++)
                {
                    s[k][j] = (char*)malloc(sizeof(char) * 160);
                    memset(s[k][j], 0, sizeof(char) * 160);
                }
            }
            for (k = 0; k < 288; k++)
            {
                size[k] = 0;
            }
            minvfB[i] = 0;
            Decomposition_V2(cube, dim, N0, s, minvfA, minvfB, minvfC, size);
            findeg = computeDPDegree(cube, dim, s, size, Round, N0);
            if (findeg == 1)
            {
                minvfB[i] = 1;
            }
            for (j = 0; j < 288; j++)
            {
                for (k = 0; k < 10000; k++)
                {
                    free(s[j][k]);
                }
                free(s[j]);
            }
            free(s);
            printf("%d %d \n", i, findeg);
        }
        if (vfC[i] == 1)
        {
            //char s[288][10000][160] = {0};  
            char*** s;

            s = (char***)malloc(sizeof(char**) * 288);
            for (k = 0; k < 288; k++)
            {
                s[k] = (char**)malloc(sizeof(char*) * 10000);
                for (j = 0; j < 10000; j++)
                {
                    s[k][j] = (char*)malloc(sizeof(char) * 160);
                    memset(s[k][j], 0, sizeof(char) * 160);
                }
            }
            for (k = 0; k < 288; k++)
            {
                size[k] = 0;
            }
            minvfC[i] = 0;
            Decomposition_V2(cube, dim, N0, s, minvfA, minvfB, minvfC, size);
            findeg = computeDPDegree(cube, dim, s, size, Round, N0);
            if (findeg == 1)
            {
                minvfC[i] = 1;
            }
            for (j = 0; j < 288; j++)
            {
                for (k = 0; k < 10000; k++)
                {
                    free(s[j][k]);
                }
                free(s[j]);
            }
            free(s);
            printf("%d %d \n", i, findeg);
        }
        //printf("\n");
    }
    printf("\n");
    for (i = 0; i < N0; i++)
    {
        if ((minvfA[i] == 1) || (minvfB[i] == 1) || (minvfC[i] == 1))
        {
            bsnum++;
        }
    }


    fopen_s(&fpout, "Res.txt", "a+");
    fprintf(fpout, "%d basis are needed: ", bsnum);

    for (i = 0; i < N0; i++)
    {
        if ((minvfA[i] == 1) || (minvfB[i] == 1) || (minvfC[i] == 1))
        {
            fprintf(fpout, "%d ", i);
        }
    }
    fprintf(fpout, "\n");
    fprintf(fpout, "\n");
    fprintf(fpout, "\n");
    fclose(fpout);
}



int main()
{
    int roundnum = 844;
    int dim = 38;
    int N0 = 150;


    FILE* fp, * fpout, * fresult;
    fopen_s(&fp, "cube.txt", "r");
    char buff[1000] = { 0 };
    int X[80] = { 0 };
    int m, l = 0;
    char c = fgetc(fp);
    int deg;
    int i, j, k;


    while (c != EOF)
    {

        //char s[288][10000][160] = {0};
        char*** s;
        s = (char***)malloc(sizeof(char**) * 288);
        for (i = 0; i < 288; i++)
        {
            s[i] = (char**)malloc(sizeof(char*) * 10000);
            for (j = 0; j < 10000; j++)
            {
                s[i][j] = (char*)malloc(sizeof(char) * 160);
                memset(s[i][j], 0, sizeof(char) * 160);
            }
        }
        int size[288] = { 0 };              //Record the number of monomials for each internal state bit

        //  read file: cube
        memset(buff, 0, sizeof(char) * 1000);
        j = 0;
        while (c != '\n')
        {
            buff[j++] = c;
            c = fgetc(fp);
        }
        c = fgetc(fp);
        m = 0;
        i = 0;
        printf("\n**********************************************************\n");
        printf("%d: ", l);
        while (i < j)
        {
            if ((buff[i] >= '0') && (buff[i] <= '9'))
            {
                if (buff[i + 1] == ',')
                {
                    X[m++] = buff[i] - '0';
                    i = i + 2;
                }
                else
                {
                    if (buff[i + 2] == ',')
                    {
                        X[m++] = (buff[i] - '0') * 10 + buff[i + 1] - '0';
                        i = i + 3;
                    }
                    else
                    {
                        i++;
                    }
                }
            }
        }
        fopen_s(&fpout, "Res.txt", "a+");
        fprintf(fpout, "Attack %d-Trivium\n", roundnum);
        for (i = 0; i < dim; i++)
        {
            fprintf(fpout, "%d,", X[i]);
        }
        fprintf(fpout, "\n");
        fclose(fpout);

        int vfA[300] = { 0 }, vfB[300] = { 0 }, vfC[300] = { 0 };
        Decomposition(X, dim, N0, s, vfA, vfB, vfC, size);
        deg = computeDPDegree(X, dim, s, size, roundnum, N0);
        printf("%d \n", deg);

        if (deg == -1)
        {
            fopen_s(&fresult, "result.txt", "a+");
            for (i = 0; i < dim; i++)
            {
                fprintf(fresult, "%d,", X[i]);
            }
            fprintf(fresult, "\n");
            fclose(fresult);
            minimize_basis(X, dim, roundnum, vfA, vfB, vfC, N0, size);
        }


        l++;

        for (j = 0; j < 288; j++)
        {
            for (i = 0; i < 10000; i++)
            {
                free(s[j][i]);
            }
            free(s[j]);
        }
        free(s);
    }

    fclose(fp);
    system("pause");
    return 0;
}
