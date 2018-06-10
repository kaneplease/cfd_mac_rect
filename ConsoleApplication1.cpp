// ConsoleApplication1.cpp : �R���\�[�� �A�v���P�[�V�����̃G���g�� �|�C���g���`���܂��B
//

#include "stdafx.h"
#include <stdio.h>
#include <math.h>
#include <iostream> //���o�̓��C�u����
#include <fstream> //iostream�̃t�@�C�����o�͂��T�|�[�g

using namespace std;

//���̂̏���
double  re = 70.0;
double  cfl = 0.2;
double  omegap = 1.00;
int     maxitp = 100;
double  errorp = 1.0e-4;
int     nlast = 5000;
int     nlp = 10;

//�v�Z�i�q�̐ݒ�
#define mx  401
int     i_1 = 96;
int     i_2 = 106;
int     i_3 = 196;
int     i_4 = 206;

#define my  201
int     j_1 = 96;
int     j_2 = 106;
int     j_3 = 106;
int     j_4 = 116;

//�֐��̐錾
void intcnd(double u_dummy[][my + 2], double v_dummy[][my + 2], double p_dummy[][my + 2]);
void bcforp(double p_dummy[][my + 2]);
void bcforv(double u_dummy[][my + 2], double v_dummy[][my + 2]);
int poiseq(double u_dummy[][my + 2], double v_dummy[][my + 2], double p_dummy[][my + 2], double dx_dummy, double dy_dummy, double dt_dummy);
void veloeq(double u_dummy[][my + 2], double v_dummy[][my + 2], double p_dummy[][my + 2], double dx_dummy, double dy_dummy, double dt_dummy);

//template <typename T>
//void p(T s) {
//    std::cout << s << std::endl;
//}

int main()
{
    //�v�Z�i�q�̐ݒ�i���́j
    double  dx = 1.0 / (double)(i_2 - i_1);
    double  dy = 1.0 / (double)(j_2 - j_1);
    int     icent = (i_1 + i_2) / 2;
    int     jcent = (j_1 + j_2) / 2;

    double  x[mx + 1][my + 1];
    double  y[mx + 1][my + 1];

    {
        for (int i = 1; i < mx + 1; i++) {
            for (int j = 1; j < my + 1; j++) {
                x[i][j] = dx*(double)(i - icent);
                y[i][j] = dy*(double)(j - jcent);
            }
        }
    }

    //{
    //    for (int i; i < mx; i++) {
    //        x[i] = dx*(double)(i - icent);
    //    }
    //    for (int j; j < my; j++) {
    //        y[j] = dy*(double)(j - jcent);
    //    }
    //}
    //�v�Z�i�q�̐ݒ�I��

    //�N�[��������
    double dt = cfl*fmin(dx, dy);

    ////////////////////////////////////////////////////////////////////////////
    //���������
    ////////////////////////////////////////////////////////////////////////////
    double  u[mx + 2][my + 2];
    double  v[mx + 2][my + 2];
    double  p[mx + 2][my + 2];

    //���������̐ݒ�
    //int     n = 0;
    //double  time = 0.0;
    intcnd(u, v, p);
    bcforp(p);
    bcforv(u, v);

    //����������������
    /*for (int i = 0; i < mx; i++) {
    for (int j = 0; j < my; j++) {
    printf("%f", u[i][j]);
    }
    putchar('\n');
    }*/

    int itr_num;

    //�T�C�N���v�Z
    for (int n = 1; n < nlast + 1; n++) {

        itr_num = poiseq(u, v, p, dx, dy, dt);
        bcforp(p);

        veloeq(u, v, p, dx, dy, dt);
        bcforv(u, v);

        if (n % 10 == 0) {
            printf("%d,%d\n", n, itr_num);
        }
        if (n % 50 == 0) {
            //.data�ɏ����o��
            {
                ofstream ofs("Result.data" ,std::ios::out | std::ios::app);
                for (int i = 1; i < mx + 1; i++) {
                    for (int j = 1; j < my + 1; j++) {
                        ofs << x[i][j] << " " << y[i][j] << " " << 2.0*p[i][j] << endl;
                    }
                    ofs << endl;
                }
                ofs << endl;
                ofs << endl;
            }
        }
    }

    //���͂𐳂����l�ɕϊ�����
    for (int i = 1; i < mx + 1; i++) {
        for (int j = 1; j < my + 1; j++) {
            p[i][j] = 2.0*p[i][j];
        }
    }


    ////csv�ɏ����o��
    //{
    //    ofstream ofs("Result_x.csv");
    //    for (int i = 1; i < mx + 1; i++) {
    //        ofs << x[i][1] << ",";
    //    }
    //    ofs << endl;
    //}
    //{
    //    ofstream ofs("Result_y.csv");
    //    for (int j = 1; j < my + 1; j++) {
    //        ofs << y[1][j] << ",";
    //    }
    //    ofs << endl;
    //}
    //{
    //    ofstream ofs("Result_p.csv");
    //    for (int i = 1; i < mx + 1; i++) {
    //        for (int j = 1; j < my + 1; j++) {
    //            ofs << 2.0*p[i][j] << ",";
    //        }
    //        ofs << endl;
    //    }
    //    ofs << endl;
    //}

    double cd = 0.0;
    {
        for (int j = j_1; j < j_2; j++) {
            static double cpfore = (2.0*p[i_1][j] + 2.0*p[i_1][j + 1]) / 2.0;
            static double cpback = (2.0*p[i_2][j] + 2.0*p[i_2][j + 1]) / 2.0;
            cd = cd + (cpfore - cpback)*dy;
        }
    }
    printf("%f", cd);

    double cl = 0.0;
    {
        for (int i = i_1; i < i_2; i++) {
            static double cpbtm = (2.0*p[i][j_1] + 2.0*p[i + 1][j_1]) / 2.0;
            static double cptop = (2.0*p[i][j_2] + 2.0*p[i + 1][j_2]) / 2.0;
            cl = cl + (cpbtm - cptop)*dx;
        }
    }
    printf("%f", cl);

    printf("����\n");
    return 0;

}

/*���������̐ݒ�*/
void intcnd(double u_dummy[][my + 2], double v_dummy[][my + 2], double p_dummy[][my + 2])
{
    for (int i = 0; i < mx + 2; i++) {
        for (int j = 0; j < my + 2; j++) {
            u_dummy[i][j] = 1.0;
            v_dummy[i][j] = 0.0;
            p_dummy[i][j] = 0.0;
        }
    }
}

/*���͂̋��E�����̐ݒ�*/
void bcforp(double p_dummy[][my + 2])
{
    {
        int i = 1;
        for (int j = 1; j < my + 1; j++) {
            p_dummy[i][j] = 0.0;
        }
    }
    {
        int i = mx;
        for (int j = 1; j < my + 1; j++) {
            p_dummy[i][j] = 0.0;
        }
    }
    {
        int j = 1;
        for (int i = 1; i < mx + 1; i++) {
            p_dummy[i][j] = 0.0;
        }
    }
    {
        int j = my;
        for (int i = 1; i < mx + 1; i++) {
            p_dummy[i][j] = 0.0;
        }
    }

    //�ǎ���̋��E����
    p_dummy[i_1][j_1] = p_dummy[i_1 - 1][j_1 - 1];
    p_dummy[i_1][j_2] = p_dummy[i_1 - 1][j_2 + 1];
    p_dummy[i_2][j_1] = p_dummy[i_2 + 1][j_1 - 1];
    p_dummy[i_2][j_2] = p_dummy[i_2 + 1][j_2 + 1];

    p_dummy[i_3][j_3] = p_dummy[i_3 - 1][j_3 - 1];
    p_dummy[i_3][j_4] = p_dummy[i_3 - 1][j_4 + 1];
    p_dummy[i_4][j_3] = p_dummy[i_4 + 1][j_3 - 1];
    p_dummy[i_4][j_4] = p_dummy[i_4 + 1][j_4 + 1];
    {
        int i = i_1;
        for (int j = j_1 + 1; j < j_2; j++) {
            p_dummy[i][j] = p_dummy[i - 1][j];
        }
    }
    {
        int i = i_2;
        for (int j = j_1 + 1; j < j_2; j++) {
            p_dummy[i][j] = p_dummy[i + 1][j];
        }
    }
    {
        int j = j_1;
        for (int i = i_1 + 1; i < i_2; i++) {
            p_dummy[i][j] = p_dummy[i][j - 1];
        }
    }
    {
        int j = j_2;
        for (int i = i_1 + 1; i < i_2; i++) {
            p_dummy[i][j] = p_dummy[i][j + 1];
        }
    }

    {
        int i = i_3;
        for (int j = j_3 + 1; j < j_4; j++) {
            p_dummy[i][j] = p_dummy[i - 1][j];
        }
    }
    {
        int i = i_4;
        for (int j = j_3 + 1; j < j_4; j++) {
            p_dummy[i][j] = p_dummy[i + 1][j];
        }
    }
    {
        int j = j_3;
        for (int i = i_3 + 1; i < i_4; i++) {
            p_dummy[i][j] = p_dummy[i][j - 1];
        }
    }
    {
        int j = j_4;
        for (int i = i_3 + 1; i < i_4; i++) {
            p_dummy[i][j] = p_dummy[i][j + 1];
        }
    }
}

/*���x�̋��E�����̐ݒ�*/
void bcforv(double u_dummy[][my + 2], double v_dummy[][my + 2])
{
    {
        int i = 1;
        for (int j = 1; j < my + 1; j++) {
            u_dummy[i][j] = 1.0;
            v_dummy[i][j] = 0.0;
            u_dummy[i - 1][j] = 1.0;
            v_dummy[i - 1][j] = 0.0;
        }
    }

    {
        int i = mx;
        for (int j = 1; j < my + 1; j++) {
            u_dummy[i][j] = 2.0*u_dummy[i - 1][j] - u_dummy[i - 2][j];
            v_dummy[i][j] = 2.0*v_dummy[i - 1][j] - v_dummy[i - 2][j];
            u_dummy[i + 1][j] = 2.0 * u_dummy[i][j] - u_dummy[i - 1][j];
            v_dummy[i + 1][j] = 2.0 * v_dummy[i][j] - v_dummy[i - 1][j];
        }
    }

    {
        int j = 1;
        for (int i = 1; i < mx + 1; i++) {
            u_dummy[i][j] = 2.0*u_dummy[i][j + 1] - u_dummy[i][j + 2];
            v_dummy[i][j] = 2.0*v_dummy[i][j + 1] - v_dummy[i][j + 2];
            u_dummy[i][j - 1] = 2.0*u_dummy[i][j] - u_dummy[i][j + 1];
            v_dummy[i][j - 1] = 2.0*v_dummy[i][j] - v_dummy[i][j + 1];
        }
    }
    {
        int j = my;
        for (int i = 1; i < mx + 1; i++) {
            u_dummy[i][j] = 2.0*u_dummy[i][j - 1] - u_dummy[i][j - 2];
            v_dummy[i][j] = 2.0*v_dummy[i][j - 1] - v_dummy[i][j - 2];
            u_dummy[i][j + 1] = 2.0*u_dummy[i][j] - u_dummy[i][j - 1];
            v_dummy[i][j + 1] = 2.0*v_dummy[i][j] - v_dummy[i][j - 1];
        }
    }
    for (int i = i_1; i < i_2 + 1; i++) {
        for (int j = j_1; j < j_2 + 1; j++) {
            u_dummy[i][j] = 0.0;
            v_dummy[i][j] = 0.0;
        }
    }

    for (int i = i_3; i < i_4 + 1; i++) {
        for (int j = j_3; j < j_4 + 1; j++) {
            u_dummy[i][j] = 0.0;
            v_dummy[i][j] = 0.0;
        }
    }

}

/*���͏������*/
int poiseq(double u_dummy[][my + 2], double v_dummy[][my + 2], double p_dummy[][my + 2], double dx_dummy, double dy_dummy, double dt_dummy)
{
    static double rhs[mx + 2][my + 2];

    //RHS���v�Z����
    {
        for (int i = 2; i < mx; i++) {
            for (int j = 2; j < my; j++) {
                if ((i_1 <= i && i <= i_2) && (j_1 <= j && j <= j_2)) {
                    continue;
                }

                else if ((i_3 <= i && i <= i_4) && (j_3 <= j && j <= j_4)) {
                    continue;
                }
                else {
                    double ux = (u_dummy[i + 1][j] - u_dummy[i - 1][j]) / (2.0*dx_dummy);
                    double uy = (u_dummy[i][j + 1] - u_dummy[i][j - 1]) / (2.0*dy_dummy);
                    double vx = (v_dummy[i + 1][j] - v_dummy[i - 1][j]) / (2.0*dx_dummy);
                    double vy = (v_dummy[i][j + 1] - v_dummy[i][j - 1]) / (2.0*dy_dummy);

                    rhs[i][j] = (ux + vy) / dt_dummy - (pow(ux, 2.0) + 2.0*uy*vx + pow(vy, 2.0));
                }
            }
        }
    }
    //iteraion����
    double res = 0.0;
    int itr_final = 0;
    {

        for (int itr = 1; itr < maxitp + 1; itr++) {
            res = 0.0;

            for (int i = 2; i < mx; i++) {
                for (int j = 2; j < my; j++) {
                    //���̓����̎��͌v�Z���Ȃ�
                    if ((i_1 <= i && i <= i_2) && (j_1 <= j && j <= j_2)) {
                        //�������Ȃ�
                    }

                    else if ((i_3 <= i && i <= i_4) && (j_3 <= j && j <= j_4)) {
                        continue;
                    }
                    //����ȊO�̎�
                    else {
                        double dp;
                        dp = (p_dummy[i + 1][j] + p_dummy[i - 1][j]) / pow(dx_dummy, 2.0)
                            + (p_dummy[i][j + 1] + p_dummy[i][j - 1]) / pow(dy_dummy, 2.0)
                            - rhs[i][j];
                        dp = dp / (2.0 / pow(dx_dummy, 2.0) + 2.0 / pow(dy_dummy, 2.0)) - p_dummy[i][j];
                        res = res + pow(dp, 2.0);
                        p_dummy[i][j] = p_dummy[i][j] + omegap*dp;
                    }

                }

            }
            //���E�����̐ݒ�
            bcforp(p_dummy);

            res = sqrt(res / ((double)(mx*my)));
            itr_final = itr;
            //printf("%f", res);

            //�c�����\���������Ƃ����[�v����E�o
            if (res < errorp) {
                //printf("�E�o���܂�");
                break;
            }
            itr_final = itr;
        }

    }
    return itr_final;
}

void veloeq(double u_dummy[][my + 2], double v_dummy[][my + 2], double p_dummy[][my + 2], double dx_dummy, double dy_dummy, double dt_dummy)
{
    static double urhs[mx + 2][my + 2];
    static double vrhs[mx + 2][my + 2];

    //���͌��z
    {
        for (int i = 2; i < mx; i++) {
            for (int j = 2; j < my; j++) {
                if ((i_1 <= i && i <= i_2) && (j_1 <= j && j <= j_2)) {
                    continue;
                }

                else if ((i_3 <= i && i <= i_4) && (j_3 <= j && j <= j_4)) {
                    continue;
                }
                else {
                    urhs[i][j] = -(p_dummy[i + 1][j] - p_dummy[i - 1][j]) / (2.0*dx_dummy);
                    vrhs[i][j] = -(p_dummy[i][j + 1] - p_dummy[i][j - 1]) / (2.0*dy_dummy);
                }
            }
        }
    }

    //�S����
    {
        for (int i = 2; i < mx; i++) {
            for (int j = 2; j < my; j++) {
                if ((i_1 <= i && i <= i_2) && (j_1 <= j && j <= j_2)) {
                    continue;
                }

                else if ((i_3 <= i && i <= i_4) && (j_3 <= j && j <= j_4)) {
                    continue;
                }
                else {
                    urhs[i][j] = urhs[i][j]
                        + (u_dummy[i + 1][j] - 2.0*u_dummy[i][j] + u_dummy[i - 1][j]) / (re*pow(dx_dummy, 2.0))
                        + (u_dummy[i][j + 1] - 2.0*u_dummy[i][j] + u_dummy[i][j - 1]) / (re*pow(dy_dummy, 2.0));
                    vrhs[i][j] = vrhs[i][j]
                        + (v_dummy[i + 1][j] - 2.0*v_dummy[i][j] + v_dummy[i - 1][j]) / (re*pow(dx_dummy, 2.0))
                        + (v_dummy[i][j + 1] - 2.0*v_dummy[i][j] + v_dummy[i][j - 1]) / (re*pow(dy_dummy, 2.0));
                }
            }
        }
    }

    //�ڗ����ix���ɂ���)
    {
        for (int j = j_1 + 1; j < j_2; j++) {
            u_dummy[i_1 + 1][j] = 2.0*u_dummy[i_1][j] - u_dummy[i_1 - 1][j];
            u_dummy[i_2 - 1][j] = 2.0*u_dummy[i_2][j] - u_dummy[i_2 + 1][j];
            v_dummy[i_1 + 1][j] = 2.0*v_dummy[i_1][j] - v_dummy[i_1 - 1][j];
            v_dummy[i_2 - 1][j] = 2.0*v_dummy[i_2][j] - v_dummy[i_2 + 1][j];
        }

    }
    {
        for (int i = 2; i < mx; i++) {
            for (int j = 2; j < my; j++) {
                if ((i_1 <= i && i <= i_2) && (j_1 <= j && j <= j_2)) {
                    continue;
                }

                else if ((i_3 <= i && i <= i_4) && (j_3 <= j && j <= j_4)) {
                    continue;
                }
                else {
                    urhs[i][j] = urhs[i][j]
                        - u_dummy[i][j]
                        * (-u_dummy[i + 2][j] + 8.0*(u_dummy[i + 1][j] - u_dummy[i - 1][j]) + u_dummy[i - 2][j])
                        / (12.0*dx_dummy)
                        - fabs(u_dummy[i][j])
                        *(u_dummy[i + 2][j] - 4.0*u_dummy[i + 1][j] + 6.0*u_dummy[i][j] - 4.0*u_dummy[i - 1][j] + u_dummy[i - 2][j])
                        / (4.0*dx_dummy);

                    vrhs[i][j] = vrhs[i][j]
                        - u_dummy[i][j]
                        * (-v_dummy[i + 2][j] + 8.0*(v_dummy[i + 1][j] - v_dummy[i - 1][j]) + v_dummy[i - 2][j])
                        / (12.0*dx_dummy)
                        - fabs(u_dummy[i][j])
                        *(v_dummy[i + 2][j] - 4.0*v_dummy[i + 1][j] + 6.0*v_dummy[i][j] - 4.0*v_dummy[i - 1][j] + v_dummy[i - 2][j])
                        / (4.0*dx_dummy);
                }
            }
        }
    }

    //�ڗ����iy���j
    {
        for (int i = i_1 + 1; i < i_2; i++) {
            u_dummy[i][j_1 + 1] = 2.0*u_dummy[i][j_1] - u_dummy[i][j_1 - 1];
            u_dummy[i][j_2 - 1] = 2.0*u_dummy[i][j_2] - u_dummy[i][j_2 + 1];
            v_dummy[i][j_1 + 1] = 2.0*v_dummy[i][j_1] - v_dummy[i][j_1 - 1];
            v_dummy[i][j_2 - 1] = 2.0*v_dummy[i][j_2] - v_dummy[i][j_2 + 1];
        }

    }
    {
        for (int i = i_3 + 1; i < i_4; i++) {
            u_dummy[i][j_3 + 1] = 2.0*u_dummy[i][j_3] - u_dummy[i][j_3 - 1];
            u_dummy[i][j_4 - 1] = 2.0*u_dummy[i][j_4] - u_dummy[i][j_4 + 1];
            v_dummy[i][j_3 + 1] = 2.0*v_dummy[i][j_3] - v_dummy[i][j_3 - 1];
            v_dummy[i][j_4 - 1] = 2.0*v_dummy[i][j_4] - v_dummy[i][j_4 + 1];
        }

    }
    {
        for (int i = 2; i < mx; i++) {
            for (int j = 2; j < my; j++) {
                if ((i_1 <= i && i <= i_2) && (j_1 <= j && j <= j_2)) {
                    continue;
                }
                else if ((i_3 <= i && i <= i_4) && (j_3 <= j && j <= j_4)) {
                    continue;
                }
                else {
                    urhs[i][j] = urhs[i][j]
                        - v_dummy[i][j]
                        * (-u_dummy[i][j + 2] + 8.0*(u_dummy[i][j + 1] - u_dummy[i][j - 1]) + u_dummy[i][j - 2])
                        / (12.0*dy_dummy)
                        - fabs(v_dummy[i][j])
                        *(u_dummy[i][j + 2] - 4.0*u_dummy[i][j + 1] + 6.0*u_dummy[i][j] - 4.0*u_dummy[i][j - 1] + u_dummy[i][j - 2])
                        / (4.0*dy_dummy);

                    vrhs[i][j] = vrhs[i][j]
                        - v_dummy[i][j]
                        * (-v_dummy[i][j + 2] + 8.0*(v_dummy[i][j + 1] - v_dummy[i][j - 1]) + v_dummy[i][j - 2])
                        / (12.0*dy_dummy)
                        - fabs(v_dummy[i][j])
                        *(v_dummy[i][j + 2] - 4.0*v_dummy[i][j + 1] + 6.0*v_dummy[i][j] - 4.0*v_dummy[i][j - 1] + v_dummy[i][j - 2])
                        / (4.0*dy_dummy);
                }
            }
        }
    }

    //update
    {
        for (int i = 2; i < mx; i++) {
            for (int j = 2; j < my; j++) {
                if ((i_1 <= i && i <= i_2) && (j_1 <= j && j <= j_2)) {
                    continue;
                }

                else if ((i_3 <= i && i <= i_4) && (j_3 <= j && j <= j_4)) {
                    continue;
                }
                else {
                    u_dummy[i][j] = u_dummy[i][j] + dt_dummy*urhs[i][j];
                    v_dummy[i][j] = v_dummy[i][j] + dt_dummy*vrhs[i][j];
                }
            }
        }

    }

}