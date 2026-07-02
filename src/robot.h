#ifndef ROBOT_H
#define ROBOT_H

/* ロボットのZ軌跡アニメーションを初期位置に戻す(トピック画面開始時に呼ぶ) */
void robot_init(void);

/* 1ティックぶんアニメーションを進める。軌跡の終端に達したら以後は静止する */
void robot_step(void);

#endif
