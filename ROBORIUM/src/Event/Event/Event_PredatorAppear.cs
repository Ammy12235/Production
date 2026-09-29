using UnityEngine;

// 蜘蛛を特定の位置からジャンプで登場させ、巡回ルートを変更するイベント
public class Event_PredatorAppear : IEvent
{
    [Header("対象の捕食者")]
    public PredatorAI targetPredator;

    [Header("ジャンプの設定")]
    [Tooltip("ジャンプの【開始位置】。設定しない場合は現在の位置からジャンプします")]
    public Transform jumpStartPoint; 
    
    [Tooltip("ジャンプの【着地位置】")]
    public Transform jumpTargetPoint; 
    
    public float jumpHeight = 8.0f;
    public float airTime = 0.4583333f;

    [Header("ジャンプ後の新しい巡回ルート")]
    [Tooltip("新しく巡回させたいポイントの親オブジェクトをアタッチ")]
    public Transform newPatrolRouteGroup;

    public override void Init()
    {
        base.Init();
    }

    public override void Run()
    {
        base.Run();

        if (targetPredator != null && jumpTargetPoint != null)
        {
            // 1. まず、蜘蛛の「新しい巡回ルート」を上書きする
            targetPredator.SetNewPatrolRoute(newPatrolRouteGroup);

            // 2. ジャンプ開始位置が指定されている場合、そこへ一瞬で移動させる
            if (jumpStartPoint != null)
            {
                // NavMeshAgentを安全に瞬間移動させる専用メソッド（Warp）を使用
                if (targetPredator.navMeshAgent.isOnNavMesh)
                {
                    targetPredator.navMeshAgent.Warp(jumpStartPoint.position);
                }
                else
                {
                    // 万が一NavMesh上にいない場合は直接座標を書き換える
                    targetPredator.transform.position = jumpStartPoint.position;
                }
                
                // 向きも開始位置のオブジェクトに合わせておく（より自然なジャンプになります）
                targetPredator.transform.rotation = jumpStartPoint.rotation;
            }

            // 3. ジャンプ状態に強制移行させる（着地後は新しい WanderAroundNestState へ）
            targetPredator.ChangeState(new JumpState(jumpTargetPoint.position, jumpHeight, airTime, new WanderAroundNestState()));
            
            Debug.Log($"<color=magenta>[Event]</color> 捕食者の登場イベント発動！");
        }
        else
        {
            Debug.LogWarning("Event_PredatorAppear: 捕食者、または着地位置（Jump Target Point）が設定されていません！");
        }

        // イベントは命令を出してすぐ終了する
        Finish();
    }

    public override void Finish()
    {
        base.Finish();
    }
}