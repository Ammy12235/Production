using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.ProBuilder;

public class CPU : PlayerEntity
{
    
    [SerializeField] private float thinkIntervalRandom = 5;
    [SerializeField] private List<Vector3> aiRefPos;

   
   
    float chargeDecideTimer = 0;
    float chargeDecideTime = 0;
    float thinkTime=0;
    float thinkTimer = 0;
    int currentTargetIndex;

    enum eAIPattern
    {
        eAIPattern_None = -1,
        eAIPattern_Attacker,//敵目掛けて攻撃する
        eAIPattern_WallBouncer,//壁目掛けて体当たりする
        eAIPattern_CenterOccupation,//中央に陣取る
        eAIPattern_Transformer,//途中でパターンが変化する
        eAIPatetrn_Pacifism,//攻撃しない
        eAIPattern_NoMove,//動かない
    }

    [SerializeField] private eAIPattern initAIPattern;
    eAIPattern pattern;

    Vector3 refPoint;
    

    [SerializeField]List<GameObject> targets;
    [SerializeField]List<int> targetIndeices;
  
    bool[] deadFlag = new bool[3];
    KeyCode chargeKey = KeyCode.Space;
    // Start is called before the first frame update
    public override void Start()
    {
        base.Start();
        GameManager.instance.SetOthersPlayer(this.gameObject, targets, targetIndeices);
       
        thinkTime = Random.Range(0, thinkIntervalRandom);
        pattern = initAIPattern;

        audioSource = GetComponent<AudioSource>();
        state = eState.State_Idle;
    }

    // Update is called once per frame
    public override void Update()
    {
        if (!GameManager.instance.CanPlayerMove()) return;
        
        // baseのUpdate内でHitStopの時間経過処理をしているため、先にbaseを呼ばないとここで固まるので注意
        base.Update();

        if (hitStop) return;
        
        if (pattern == eAIPattern.eAIPattern_NoMove)
        {
            refPoint = transform.position;
        }
        
        if (isMoveFree)
        {
            thinkTimer += Time.deltaTime;
            if (thinkTimer > thinkTime)
            {
                thinkTimer = 0.0f;
                thinkTime = Random.Range(0, thinkIntervalRandom);


                if (aiRefPos != null)
                {
                    if (pattern == eAIPattern.eAIPattern_Attacker)
                    {
                        refPoint = aiRefPos[Random.Range(0, aiRefPos.Count)];//ステージの端っこ
                        if (Random.Range(0, 1) == 0)
                        {
                            float nearest = 10000;
                            int nearestIndex = 0;

                            float distance = 0;
                            for (int i = 0; i < targets.Count; i++)
                            {
                                distance = Vector3.Distance(transform.position, targets[i].transform.position);
                                if (nearest > distance && !GameManager.instance.GetIsDeadbyIndex(targetIndeices[i]))
                                {
                                    nearest = distance;
                                    nearestIndex = i;
                                }
                            }
                            chargeDecideTime = Random.Range(1, chargeMaxTime);
                            currentTargetIndex = nearestIndex;
                            /*
                            while (CheckLifeState(currentTargetIndex))
                            {
                                currentTargetIndex = Random.Range(0, targets.Count - 1); 
                            }
                            */

                            state = eState.State_Charge;
                        }
                    }
                    else if (pattern == eAIPattern.eAIPattern_WallBouncer)
                    {
                        float nearest = 10000;
                        int nearestIndex = 0;

                        float distance = 0;
                        for (int i = 1; i < aiRefPos.Count; i++)
                        {
                            distance = Vector3.Distance(transform.position, aiRefPos[i]);
                            if (nearest > distance)
                            {
                                nearest = distance;
                                nearestIndex = i;
                            }
                        }
                        refPoint = aiRefPos[Random.Range(0, aiRefPos.Count)] * 0.8f;//ステージの真ん中寄りの端っこ
                        if (Random.Range(0, 2) == 0)
                        {
                            chargeDecideTime = Random.Range(1, chargeMaxTime);
                            currentTargetIndex = Random.Range(0, targets.Count - 1);
                            /*
                            while (CheckLifeState(currentTargetIndex))
                            {
                                currentTargetIndex = Random.Range(0, targets.Count - 1);
                            }
                            */
                            state = eState.State_Charge;
                        }
                    }
                    else if (pattern == eAIPattern.eAIPattern_CenterOccupation)
                    {
                        float nearest = 10000;
                        int nearestIndex = 0;

                        float distance = 0;
                        for (int i = 0; i < targets.Count; i++)
                        {
                            distance = Vector3.Distance(transform.position, targets[i].transform.position);
                            if (nearest > distance)
                            {
                                nearest = distance;
                                nearestIndex = i;
                            }
                        }
                        refPoint = aiRefPos[Random.Range(0, aiRefPos.Count)] / 10;//ステージのほぼ真ん中
                        if (Random.Range(0, 3) == 0)
                        {
                            chargeDecideTime = Random.Range(0, chargeMaxTime / 4);
                            currentTargetIndex = nearestIndex;
                            state = eState.State_Charge;

                        }
                    }
                    else if (pattern == eAIPattern.eAIPatetrn_Pacifism)
                    {
                        refPoint = aiRefPos[Random.Range(0, aiRefPos.Count)];//ステージの端っこ
                    }
                    else if (pattern == eAIPattern.eAIPattern_NoMove)
                    {
                        refPoint = transform.position;
                    }

                }
            }
            Vector3 diff = (refPoint - transform.position).normalized;
            v.x = diff.x;
            v.z = diff.z;

            v = new Vector3(v.x*moveSpeed, v.y, v.z*moveSpeed);

            if (state == eState.State_Charge)
            {
                arwMatObj.SetActive(true);
                UpdateDirEffect();

                switch (pattern)
                {
                    case eAIPattern.eAIPattern_Attacker:
                        AttackPattern();
                        break;
                    case eAIPattern.eAIPattern_WallBouncer:
                        WallBouncePattern();
                        break;
                    case eAIPattern.eAIPattern_CenterOccupation:
                        CenterOccupyPattern();
                        break;

                    default:
                        break;
                }
                
                chargeLevel = Mathf.Min((int)(chargeDecideTimer / chargeLevelInterval), 2);
                if (chargeDecideTimer > chargeDecideTime)
                {

                    boostDir = transform.forward.normalized;
                    baseBoostX = boostDir.x * (boostDistances[chargeLevel] / boostTimes[chargeLevel]);
                    baseBoostZ = boostDir.z * (boostDistances[chargeLevel] / boostTimes[chargeLevel]);
                    v.x = baseBoostX;
                    v.z = baseBoostZ;
                    if (chargeLevel > 2) chargeLevel = 2;
                    if (chargeDecideTimer > chargeMaxTime)
                    {
                        chargeLevel = 2;
                    }
                    chargeDecideTimer = 0.0f;
                    boostTimer = 0;
                    isMoveFree = false;
                    if (!isDownObjDisabled) {
                        downObjRenderer.enabled = true;
                    }

                    arwMatObj.SetActive(false);
                    state = eState.State_Boost;
                }
            }
            else
            {
                diff.y = 0;



                Quaternion rotate = Quaternion.LookRotation(diff);
                transform.rotation = Quaternion.RotateTowards(transform.rotation, rotate, 7);

            }

        }

        if (state == eState.State_Boost)
        {
            boostTimer += Time.deltaTime;
            float attenuationTiming = boostTimes[chargeLevel] * boostAttenuationStarts[chargeLevel];
            if (boostTimer > attenuationTiming)
            {
                float progress = (boostTimer - attenuationTiming) / (boostTimes[chargeLevel] * (1 - boostAttenuationStarts[chargeLevel]));
                if (progress > 1.0f) progress = 1.0f;
                float attenuatedX = Mathf.Lerp(baseBoostX, 0, progress);
                float attenuatedZ = Mathf.Lerp(baseBoostZ, 0, progress);
                v.x = attenuatedX;
                v.z = attenuatedZ;
                downObjRenderer.enabled = false;
            }
            else if (boostTimer < attenuationTiming)
            {
                UpdateStateEffect(attackColor, 0.2f, 5);
            }

            if (boostTimer > boostTimes[chargeLevel])
            {
                boostTimer = 0.0f;
                state = eState.State_BoostEnd;
            }
        }

        if (state == eState.State_BoostEnd)
        {
            boostStunTimer += Time.deltaTime;
            if (boostStunTimer > boostEndStunTimes[chargeLevel])
            {
                boostStunTimer = 0.0f;
                isMoveFree = true;
                state = eState.State_Idle;
            }
        }

        if (state == eState.State_Down || state == eState.State_DownEnd)
        {
            UpdateStateEffect(downColor, 0.2f, 5);
            downTimer += Time.deltaTime;
            if (downTimer > downTime / 2)
            {
                state = eState.State_DownEnd;
                v *= downAttenuation;
            }

            if (downTimer > downTime)
            {
                downTimer = 0.0f;
                isMoveFree = true;
                downObjRenderer.enabled = false;
                state = eState.State_Idle;
                // lastHitPlayer = null;
            }
        }

        if (state == eState.State_Buried)
        {
            buriedReviveValue += Time.deltaTime * buriedReviveSpeed / 3;
            if (buriedReviveValue >= 1.0f)
            {
                ReviveFromBuried();
            }
        }
       

    }

    public override void LateUpdate()
    {
        base.LateUpdate();
    }

    //始点ベクトルからの衝突判定
    bool isHit(Vector3 pos, float length, float angle, bool isDraw, QueryTriggerInteraction interationMode)
    {
        Vector3 vec;
        vec.x = Mathf.Cos(angle) * length;
        vec.y = 0;
        vec.z = Mathf.Sin(angle) * length;
        Ray ray = new Ray(pos, vec);

        RaycastHit hit;
        if (isDraw)
            Debug.DrawRay(pos, vec, Color.red, 1, false);
        //接触しているか
        if (Physics.Raycast(ray, out hit, length, LayerMask.GetMask("Obstacle"), interationMode))
        {
            return true;
        }
        else return false;
    }


    private void AttackPattern()
    {
        Vector3 diff;
        diff.x = targets[currentTargetIndex].transform.position.x - transform.position.x;
        diff.y = 0;
        diff.z = targets[currentTargetIndex].transform.position.z - transform.position.z;
        Quaternion hRotation = Quaternion.LookRotation(diff.normalized, Vector3.up);
        transform.rotation = Quaternion.RotateTowards(transform.rotation, hRotation, 7);
        chargeDecideTimer += Time.deltaTime;
    }

    private void WallBouncePattern()
    {
        Vector3 diff;
        diff.x = aiRefPos[currentTargetIndex].x - transform.position.x;
        diff.y = 0;
        diff.z = aiRefPos[currentTargetIndex].z - transform.position.z;
        if (!isHit(transform.position, 10, Mathf.Atan2(diff.z, diff.x), true, QueryTriggerInteraction.Collide))
        {
            currentTargetIndex = 0;
        }
        Quaternion hRotation = Quaternion.LookRotation(diff.normalized, Vector3.up);
        transform.rotation = Quaternion.RotateTowards(transform.rotation, hRotation, 7);
        chargeDecideTimer += Time.deltaTime;
    }

    private void CenterOccupyPattern()
    {
        Vector3 diff;
        diff.x = targets[currentTargetIndex].transform.position.x - transform.position.x;
        diff.y = 0;
        diff.z = targets[currentTargetIndex].transform.position.z - transform.position.z;
        Quaternion hRotation = Quaternion.LookRotation(diff.normalized, Vector3.up);
        transform.rotation = Quaternion.RotateTowards(transform.rotation, hRotation, 5);
        chargeDecideTimer += Time.deltaTime;
    }
}
