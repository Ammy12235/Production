
using System;
using System.Collections;

using TMPro;
using Unity.VisualScripting;
using UnityEngine;
using UnityEngine.InputSystem;
using UnityEngine.InputSystem.Controls;
using Random = UnityEngine.Random;

public class PlayerEntity : MonoBehaviour
{

    [SerializeField] protected float moveSpeed = 1;
    // [SerializeField] protected float chargePower = 1;
    [SerializeField, Header("チャージレベルの間隔")] protected float chargeLevelInterval = 0.6f;
    /* [SerializeField] */
    protected float chargeMaxTime = 5;
    [SerializeField, Header("衝突後の速度倍率")] protected float[] pushPowers = { 1.0f, 1.1f, 1.2f };
    [SerializeField] protected float downAttenuation = 0.99f;
    [SerializeField, Header("減衰開始タイミング(0~1 %)")] protected float[] boostAttenuationStarts = { 0.95f, 0.9f, 0.85f };
    [SerializeField, Header("ブースト時間")] protected float[] boostTimes = { 2f, 2f, 2f };
    [SerializeField, Header("ブースト距離")] protected float[] boostDistances = { 10f, 10f, 10f };
    [SerializeField, Header("ブースト後のスタン時間")] protected float[] boostEndStunTimes = { 0f, 0.5f, 1f };
    [SerializeField] protected float downTime = 1;
    [SerializeField] protected float shakeInterval;
    [SerializeField] protected float shakeAmplitude;
    [SerializeField] protected float downBoostAddValue = 0f; // 吹っ飛ばし追加の削除
    [SerializeField] protected float downBoostFactor = 1;
    [SerializeField] protected int deathPenalty = 1;
    [SerializeField] protected float buriedReviveSpeed = 0.1f;

    [SerializeField] protected TextMeshProUGUI stateText;
    [SerializeField] protected TextMeshProUGUI chargeParcent;
    [SerializeField] protected TextMeshProUGUI downBoostFactorText;

    [SerializeField, Header("衝突エフェクト")] protected GameObject[] hitEffectPrefabs;

    [SerializeField, Header("チャージエフェクト")] protected GameObject[] chargeEffectPrefabs;

    [SerializeField, Header("軌道エフェクト")] protected GameObject flyTrailEffect;
    private bool isFlyTrailEffectHold = false;
    private GameObject holdFlyTrailObject;
    [SerializeField] private float trailEffectYOffset=2;

    [SerializeField, Header("壁衝突時のエフェクト")] protected GameObject wallHitEffectPrefab;

    [SerializeField, Header("チャージ中の振動")] protected MotorData chargeMotorData;
    [SerializeField, Header("プレイヤー衝突時の振動")] protected MotorData playerHitMotorData;
    [SerializeField, Header("壁衝突時の振動")] protected MotorData wallHitMotorData;

    [SerializeField, Header("プレイヤー衝突ヒットストップ時間")] protected float[] playerHitStopDurations = { 0.1f, 0.2f, 0.25f };
    [SerializeField, Header("壁衝突ヒットストップ時間")] protected float wallHitStopDuration = 0.025f;
    [SerializeField, Header("耐久1の壁衝突ヒットストップ時間")] protected float wallBreakHitStopDuration = 0.1f;

    [SerializeField, Header("足元サークルUI")] protected GameObject footCircleUIPrefab;
    [SerializeField, Header("DownObjの無効化")] protected bool isDownObjDisabled = false;

    [SerializeField, Header("ポイント獲得時の表示用Prefab")] protected GameObject scoredPointPrefab;

    protected string[] strArr = { "Idle", "Charge", "Boost", "BoostEnd", "Down", "DownEnd", "Death" };

    protected float chargeTimer = 0;
    protected float boostTimer = 0;
    protected float downTimer = 0;
    protected float downEffectTimer = 0;
    protected float buriedReviveValue = 0.0f;
    [SerializeField] protected bool isMovefreeForCharging = false;
    [SerializeField] protected bool isMoveFree = true;
    [SerializeField] protected bool isCollide = false;
    protected bool isOver = false;
    protected bool isDead = false;

    protected Vector3 initTrans;
    protected Vector3 overFinalVector;
    protected Color downColor = Color.blue;
    protected Color attackColor = Color.red;
    protected Color chargeColor = Color.yellow;
    protected float shakeTimer = 0.0f;
    protected float shakeFactor = 0.0f;

    [SerializeField] protected WallNotify buriedWallNotify;
    [SerializeField] protected Animator anim;
    [SerializeField] protected Animator arrowAnim;

    protected int score = 0;

    protected GameObject currentChargeEffect;
    protected int lastChargeLevel = -1;

    protected float currentHitStopDuration = 0.0f;
    protected float hitStopTimer = 0.0f;
    protected bool hitStop = false;
    protected Vector3 postHitStopVelocity = Vector3.zero;

    //ペアリングされたゲームパッドやJoystick
    public InputDevice controllerDevice;

    private PadAssigner padAssigner;

    public enum eState
    {
        State_None = -1,
        State_Idle,     //ニュートラル
        State_Charge,   //チャージ
        State_Boost,    //ブースト
        State_BoostEnd, //ブースト終了（相手にぶつかったなど *壁にぶつかってもブーストは維持される）
        State_Down,     //ダウン
        State_DownEnd,  //ダウン終了
        State_Death,    //死
        State_Buried,   //埋まる(未使用)
        State_NoInput   //入力不可
    }

    public enum eLifeState
    {
        LifeState_None = -1,
        LifeState_Alive,
        LifeState_Death,
        LifeState_Revivaling,
    }

    [SerializeField] protected eState state;
    [SerializeField] protected eLifeState lifeState;

    protected Vector3 v;
    protected Vector3 boostDir;
    protected Vector3 normalVector;
    protected Vector3 contactPoint;
    [SerializeField] protected GameObject arwMatObj;
    [SerializeField] protected GameObject downMatObj;
    [SerializeField] protected GameObject deathSoundObj;

    [Serializable]
    protected struct HitAudio
    {
        public AudioClip hitSound;
        [Range(0, 1)] public float volume;
    }
    [Serializable]
    protected struct HitAudios
    {
        public HitAudio[] allSounds;
    }

    [SerializeField, Header("衝突時の音(チャージレベル別)")] protected HitAudios[] hitAudios = new HitAudios[3];
    [SerializeField] protected AudioClip chargeSound;
    [SerializeField] protected AudioClip boostSound;
    [SerializeField] protected AudioClip deathSound1;
    [SerializeField] protected AudioClip deathSound2;
    [SerializeField] protected GameObject deathCrack;
    [SerializeField] protected float deathCrackY = -1.1f;
    [SerializeField] protected SkinnedMeshRenderer[] renderers;
    [SerializeField] protected Material[] arrowMats;

    //コンポーネント群
    protected SpriteRenderer downObjRenderer;
    protected Material arwMat;
    protected Rigidbody rb;
    protected AudioSource audioSource;
    protected Collider selfCollider;

    protected Vector3 wallVec;
    protected PlayerEntity lastHitPlayer;

    [SerializeField, Header("リスポーン後の無敵時間")] protected float invincibleTime = 2.0f;
    protected float invincibleTimer = 0.0f;
    protected bool isInvincible = false;

    protected float velocityResetTimer = 0.0f;

    protected int chargeLevel = 0;
    protected float boostStunTimer = 0.0f;

    protected float baseBoostX;
    protected float baseBoostZ;

    [SerializeField] private float blinkMaxTime;//瞬きの間隔最大値
    float blinkTimer;
    float blinkTimer2;
    float blinkTime;
    float blinkTime2 = 0.25f;

    [SerializeField] private Material neonMat;



    bool isAnimDeathFlag = false;

    private WallNotify lastHitWallNotify = null;

    private GameObject footCircleUI;
    private float initialFootCircleUIScale;

    private int bonusAddScore = 0;
    private int bonusWallAddScore = 0;

    // 専用joystick接続時
    protected bool isJoystickNeutral = true;
    protected ButtonControl joystickEnterButton;

    public bool IsInvincible()
    {
        return isInvincible;
    }
    // Start is called before the first frame update
    public virtual void Start()
    {
        //ステートの初期化
        state = eState.State_Idle;
        lifeState = eLifeState.LifeState_Alive;

        //コンポーネント取得
        arwMat = arwMatObj.GetComponentInChildren<SkinnedMeshRenderer>().material;
        downObjRenderer = downMatObj.GetComponent<SpriteRenderer>();
        audioSource = GetComponent<AudioSource>();
        rb = GetComponent<Rigidbody>();
        selfCollider = GetComponent<Collider>();

        //その他セッティング
        downObjRenderer.enabled = false;
        downColor = downObjRenderer.color;
        initTrans = this.gameObject.transform.localScale;
        arwMatObj.SetActive(false);
        chargeMaxTime = chargeLevelInterval * 2;
        hitStopTimer = 0.0f;
        currentHitStopDuration = 0.0f;
        blinkTime = blinkMaxTime;
        controllerDevice = null;

        //足元のUIの設定
        footCircleUI = Instantiate(footCircleUIPrefab);
        footCircleUI.GetComponent<PlayerColorCircle>().SetFollowTarget(this.gameObject);
        footCircleUI.GetComponent<PlayerColorCircle>().SetUseStaticY(true, 0.25f);
        footCircleUI.SetActive(true);
        initialFootCircleUIScale = footCircleUI.transform.localScale.x;
    }

    // Update is called once per frame
    public virtual void Update()
    {
        //足元UIのActiveフラグ
        if (this.gameObject.activeSelf == false)
        {
            footCircleUI.SetActive(false);
        }
        else footCircleUI.SetActive(true);


        // stateText.text = strArr[(int)state];
        // stateText.text = score.ToString() + "Pts";
        // downBoostFactorText.text = "x" + downBoostFactor.ToString("f2");

        if (hitStop)
        {
            hitStopTimer += Time.deltaTime;

            //ヒットストップ状態が切れたら、保存していた速度で吹っ飛ぶ
            if (hitStopTimer >= currentHitStopDuration)
            {
                hitStop = false;
                hitStopTimer = 0.0f;
                rb.isKinematic = false;
                SetVelocitySafe(rb, postHitStopVelocity);
                v = postHitStopVelocity;
                postHitStopVelocity = Vector3.zero;
            }
            else
            {
                return;//ヒットストップ中は操作等の制御を締め出す
            }
        }

        if (invincibleTimer > 0.0f)
        {
            //無敵状態が切れたら、フラグをきる
            invincibleTimer -= Time.deltaTime;
            if (invincibleTimer <= 0.0f)
            {
                isInvincible = false;
                invincibleTimer = 0.0f;
                /*
                foreach (Renderer renderer in renderers)
                {
                    foreach (Material mat in renderer.materials)
                    {
                        mat.color = new Color(mat.color.r, mat.color.g, mat.color.b, 1.0f);
                    }
                }
                */
            }
        }

        if (velocityResetTimer > 0.0f)
        {
            //復活後、速度を０にして放免する
            velocityResetTimer -= Time.deltaTime;
            if (velocityResetTimer <= 0.0f)
            {
                velocityResetTimer = 0.0f;
                SetVelocitySafe(rb, Vector3.zero);
                isMoveFree = true;
                state = eState.State_Idle;
            }
        }

        if (state == eState.State_Charge)
        {

            renderers[0].SetBlendShapeWeight(2, 100);//勇ましい顔になる
            if (chargeLevel != lastChargeLevel)
            {
                if (currentChargeEffect != null)
                {
                    Destroy(currentChargeEffect);
                }
                //チャージに関するエフェクトを生成し、音を出す
                currentChargeEffect = Instantiate(chargeEffectPrefabs[chargeLevel], transform);
                lastChargeLevel = chargeLevel;
                if (chargeLevel != -1)
                    arwMatObj.GetComponentInChildren<SkinnedMeshRenderer>().material = arrowMats[chargeLevel];
                if (chargeSound != null && chargeLevel == 0)
                {
                    audioSource.clip = chargeSound;
                    audioSource.Play();
                }

            }
            RegisterPadMotor(chargeMotorData);//ぶるぶるする
        }
        else if (currentChargeEffect != null)//チャージ状態じゃなくなったらチャージに関するエフェクトと音を消す
        {
            Destroy(currentChargeEffect);
            audioSource.Stop();
            currentChargeEffect = null;
            lastChargeLevel = -1;
            chargeTimer = 0.0f;
        }

        if (state == eState.State_Idle && lastHitWallNotify != null)
        {
            lastHitWallNotify = null;
        }


    }

    public virtual void LateUpdate()
    {
        //ゲームマネージャーが動いていいっていうまで動いちゃダメ
        if (!GameManager.instance.CanPlayerMove())
        {
            SetVelocitySafe(rb, Vector3.zero);
            return;
        }

        if (hitStop) return;

        //おそらく「壁に埋まる」の仕様の名残
        //埋まる以外は押し戻し判定を出す
        if (state != eState.State_Buried && selfCollider.isTrigger)
            selfCollider.isTrigger = false;

        //埋まっていて、
        if (state == eState.State_Buried && buriedWallNotify != null)
        {
            //抜け出す条件が揃えば
            if (buriedWallNotify.GetBuriedPlayer() == null || buriedWallNotify.GetBuriedPlayer() != gameObject || buriedWallNotify.GetWallScript().IsDestroyed())
            {
                //解放される
                ReviveFromBuried();
            }
        }

        if (state != eState.State_Buried)
        {
            v.y -= 0.1f;
        }
        if (!isOver)
        {
            if (state != eState.State_Charge)
            {
                SetVelocitySafe(rb, new Vector3(v.x, v.y, v.z));
            }
            else
            {
                if (isMovefreeForCharging)
                    SetVelocitySafe(rb, new Vector3(v.x, v.y, v.z));
                else
                    SetVelocitySafe(rb, new Vector3(0, v.y, 0));
            }
        }
        else
        {
            SetVelocitySafe(rb, overFinalVector * 10);
        }

        anim.SetInteger("state", (int)state);
        arrowAnim.SetBool("isCharge", false);

        //シェイプキーはどうやらアニメーションの後に変更しないといけないらしい
        if (anim.GetCurrentAnimatorStateInfo(0).fullPathHash == Animator.StringToHash("Base Layer.down"))
        {
            transform.LookAt(Camera.main.transform.position);

            renderers[0].SetBlendShapeWeight(0, 100);
        }
        else if (state == eState.State_Charge)
        {
            arrowAnim.SetBool("isCharge", true);
            renderers[0].SetBlendShapeWeight(2, 100);
        }
        else
        {
            //瞬きアニメーションを三角関数で制御
            blinkTimer += Time.deltaTime;
            if (blinkTimer > blinkTime)
            {
                blinkTimer2 += Time.deltaTime * Mathf.PI * 2;
                renderers[0].SetBlendShapeWeight(1, Mathf.Sin(blinkTimer2) * 100);
                if (blinkTimer2 >= Mathf.PI)
                {
                    blinkTimer2 = 0.0f;
                    blinkTimer = 0.0f;
                    blinkTime = Random.Range(0, blinkMaxTime);
                    renderers[0].SetBlendShapeWeight(1, 0);
                }
            }
        }

        if (state == eState.State_Boost || state == eState.State_BoostEnd || state == eState.State_Down ||
            state == eState.State_DownEnd)
        {
            if (isFlyTrailEffectHold == false)
            {
                isFlyTrailEffectHold = true;
                if (holdFlyTrailObject == null)
                {
                    holdFlyTrailObject = Instantiate(flyTrailEffect, transform.position, Quaternion.identity);
     
                }
            }
        }
        else
        {
            isFlyTrailEffectHold = false;
        }

        if (isFlyTrailEffectHold&& holdFlyTrailObject != null)
        {
            holdFlyTrailObject.transform.position = gameObject.transform.position;
            float dirToForward = Mathf.Atan2(rb.linearVelocity.x, rb.linearVelocity.z) * Mathf.Rad2Deg;
            holdFlyTrailObject.transform.rotation = Quaternion.Euler(new Vector3(0, dirToForward + 90, 0));
        }
        else
        {

            if (holdFlyTrailObject != null)
            {
                GameObject destroyTrail=holdFlyTrailObject;
                Destroy(destroyTrail, 5);
                holdFlyTrailObject = null;
            }
        }
    }

    public virtual void Respawn()
    {
        if (padAssigner != null)
        {
            //死んだときにPlayerInputが無効になるから再アサインしてる。。？
            padAssigner.RePairDevice(this.GetComponent<PlayerInput>(), controllerDevice);
        }
        isAnimDeathFlag = false;
        isOver = false;
        isCollide = false;
        isMoveFree = true;
        downTimer = 0.0f;
        downEffectTimer = 0.0f;
        downObjRenderer.enabled = false;
        v = Vector3.zero;
        boostDir = Vector3.zero;
        state = eState.State_Idle;
        lifeState = eLifeState.LifeState_Alive;

        shakeTimer = 0.0f;
        shakeFactor = 0.0f;

        if (buriedWallNotify != null)
        {
            buriedWallNotify.ClearBuriedPlayer();
            buriedWallNotify = null;
        }

        selfCollider.isTrigger = false;
        transform.rotation = Quaternion.identity;
        lastHitPlayer = null;
        this.gameObject.GetComponent<SphereCollider>().enabled = true;
        anim.SetInteger("lifeState", (int)lifeState);
        this.gameObject.SetActive(true);
        arwMatObj.SetActive(false);
        rb.isKinematic = false;
        downMatObj.SetActive(true);

        buriedReviveValue = 0.0f;
        isInvincible = true;
        invincibleTimer = invincibleTime;
        velocityResetTimer = 0.0f;

        chargeLevel = 0;
        boostStunTimer = 0.0f;

        hitStop = false;
        hitStopTimer = 0.0f;
        currentHitStopDuration = 0.0f;
        postHitStopVelocity = Vector3.zero;

        foreach (Renderer renderer in renderers)
        {
            renderer.enabled = true;
        }

        ActivateFootUI(1f);

        StopAllPadMotor();

        if (currentChargeEffect != null)
        {
            Destroy(currentChargeEffect);
            currentChargeEffect = null;
            lastChargeLevel = -1;
            chargeTimer = 0.0f;
        }
    }

    protected void UpdateStateEffect(Color color, float interval, float amplitude)
    {

        if (downEffectTimer > interval)
        {
            downEffectTimer = 0.0f;
            downObjRenderer.color = color;
        }


        if (downEffectTimer > interval * 4 / 5)
        {
            downObjRenderer.color -= new Color(0, 0, 0, interval / 5 * Time.deltaTime);
        }
        if (!hitStop && downObjRenderer.enabled)
            downEffectTimer += Time.deltaTime;
    }





    public void SetFootCircleUIEnabled(bool isEnabled)
    {
        footCircleUI.SetActive(isEnabled);
    }
    protected void UpdateDirEffect()
    {
        /*
        float scroll = Mathf.Repeat(Time.time, 1);
        Vector2 offset = new Vector2(0, -scroll);
         */
        //arwMat.SetTextureOffset("_MainTex", offset);

    }

    protected void OnCollisionStay(Collision collision)
    {
        if (hitStop) return;
        foreach (ContactPoint contact in collision.contacts)
        {
            // 衝突点の法線ベクトルを取得
            normalVector = contact.normal;
            // 衝突点の位置
            contactPoint = contact.point;

        }

        if (collision.gameObject.tag == "Wall" && !isCollide && state != eState.State_Buried)
        {
            Debug.Log($"Collision with wall: {gameObject.name} with {collision.gameObject.name}");
            if (state == eState.State_Boost ||
                state == eState.State_BoostEnd ||
                state == eState.State_Down ||
                state == eState.State_DownEnd)
            {
                RegisterPadMotor(wallHitMotorData);
                WallNotify script = collision.gameObject.GetComponent<WallNotify>();

                //前回衝突時と同じ壁ならすでに衝突したとみなし、抜ける（多重衝突回避用）
                if (script == lastHitWallNotify)
                {
                    isCollide = true;
                    return;
                }
                lastHitWallNotify = script;

                //壁に耐久力が残っていたら
                if (script.GetWallScript().GetHP() > 1)
                {
                    //壁の法線ベクトルを基準にし、速度を維持して反射する
                    // v = Vector3.Reflect(v, normalVector);
                    SetHitStop(Vector3.Reflect(v, normalVector), wallHitStopDuration);
                    if (script.GetWallScript().reaction(gameObject))
                    {
                        //壁に当たって跳ね返る時のエフェクト
                        Instantiate(wallHitEffectPrefab, contactPoint, Quaternion.LookRotation(normalVector, Vector3.up));
                    }
                }
                else//壁が壊れるとき
                {
                    // if (state == eState.State_Boost || state == eState.State_BoostEnd)
                    // { 
                    // v = Vector3.Reflect(v, normalVector);
                    SetHitStop(Vector3.Reflect(v, normalVector), wallBreakHitStopDuration);
                    // }
                    if (script.GetWallScript().reaction(gameObject, false))
                    {
                        //破片のパーティクルを出す
                        Instantiate(wallHitEffectPrefab, contactPoint, Quaternion.LookRotation(normalVector, Vector3.up));

                        //ぶつかった壁がスペシャル壁ならば、
                        if (script.GetWallScript().GetIsSpecial())
                        {
                            //自分がダウン状態のとき
                            if (state == eState.State_Down || state == eState.State_DownEnd)
                            {
                                //最後にぶつかってきた相手のスコアを加算する
                                lastHitPlayer.AddScore(script.GetWallScript().GetSpecialWallScore() + lastHitPlayer.GetBonusWallScore());
                            }
                            //自分からぶつかりに行ったとき
                            else
                            {
                                //自分のスコアを加算する
                                AddScore(script.GetWallScript().GetSpecialWallScore() + bonusWallAddScore);
                            }
                        }
                    }

                    // 埋まる
                    // if (script.GetBuriedPlayer() != null)
                    // {
                    //     // Debug.Log($"Already buried player: {script.GetBuriedPlayer().name} with {collision.gameObject.name}");
                    //     PlayerEntity buriedScript = script.GetBuriedPlayer().GetComponent<PlayerEntity>();
                    //     if (buriedScript != null && buriedScript.GetState() == eState.State_Buried)
                    //     {
                    //         buriedScript.SetLastHitPlayer(this);
                    //         buriedScript.applyBuriedPhysics(gameObject);
                    //     }
                    //     v = Vector3.Reflect(v, normalVector);
                    // }
                    // else
                    // {
                    //     script.SetBuriedPlayer(gameObject);
                    //     buriedWallNotify = script;
                    //     state = eState.State_Buried;
                    //     selfCollider.isTrigger = true;
                    //     overFinalVector = v;
                    //     v = Vector3.zero;
                    //     // isOver = true;
                    //     isMoveFree = false;
                    //     // Debug.Log($"Buried player: {gameObject.name} with {collision.gameObject.name}");
                    //     wallVec = new Vector3(collision.transform.position.x - transform.position.x,
                    //                           0,
                    //                           collision.transform.position.z - transform.position.z).normalized;
                    //     wallVec *= 0.25f;
                    //     transform.position += wallVec;
                    //     buriedReviveValue = 0.0f;
                    // }
                }

                isCollide = true;
            }
        }

        if ((collision.gameObject.tag == "Player" || collision.gameObject.tag == "CPU") && !isCollide)
        {
            PlayerEntity script = collision.gameObject.GetComponent<PlayerEntity>();
            if (IsInvincible() || IsHitStop()) return;
            //相手がブースト状態でぶつかってきたら
            if (script.GetState() == eState.State_Boost)
            {
                //どっちもぶるぶるする
                RegisterPadMotor(playerHitMotorData);
                script.RegisterPadMotor(script.playerHitMotorData);

                //その相手を保存し、自身が壁にぶつかったときにその相手に得点が入るようにする
                lastHitPlayer = script;
                Vector3 dirToEnemy = (script.transform.position - transform.position).normalized;

                //今は未使用
                if (state == eState.State_Buried)
                {

                    Debug.Log($"Collision to buried player: {gameObject.name} with {collision.gameObject.name}");
                    applyBuriedPhysics(collision.gameObject);
                    Vector3 enemyV = script.GetVelocity();
                    script.SetVelocity(Vector3.Reflect(enemyV, dirToEnemy));
                    return;
                }
                // v = script.GetVelocity() * downBoostFactor;
                // v = script.GetVelocity() * script.GetCurrentPushPower();

                //相手の攻撃の強度によってヒットストップ時間を決め、ヒットストップ解除後に相手のもつ速度xパワーで吹っ飛ぶよう設定
                SetHitStop(script.GetVelocity() * script.GetCurrentPushPower(), playerHitStopDurations[script.GetChargeLevel()]);
                downTimer = 0.0f;
                downEffectTimer = 0.0f;
                if (!isDownObjDisabled)
                {
                    downObjRenderer.enabled = true;
                }

                // script.SetVelocity(v * 0);

                //上記ヒットストップを相手にも適用する。
                script.SetHitStop(v * 0, playerHitStopDurations[script.GetChargeLevel()]);
                script.SetState(eState.State_BoostEnd);
                // script.SetMoveFree(true);

                GameManager.instance.generateGeneralImpulse();
                // downBoostFactor += downBoostAddValue; // 吹っ飛ばし追加の削除
                arwMatObj.SetActive(false);
                isMoveFree = false;
                isCollide = true;
                //Debug.Log("c");

                //衝突時の音を生成し、相手のぶつかった威力によって変化させて再生
                HitAudio[] allSoundsContainer = hitAudios[script.GetChargeLevel()].allSounds;
                if (allSoundsContainer.Length != 0)
                {
                    int hit = Random.Range(0, allSoundsContainer.Length);
                    HitAudio hitSoundContainer = allSoundsContainer[hit];
                    SingleSound hitSoundObj = new GameObject("HitSound").AddComponent<SingleSound>();
                    hitSoundObj.SetupSound(transform.position, hitSoundContainer.hitSound, hitSoundContainer.volume);
                }

                //down状態にし、直前にぶつかった相手からポイントをもらう権限がなくなる
                state = eState.State_Down;
                lastHitWallNotify = null;

                //相手のほうを向いてヒットエフェクトを出す
                Quaternion hitDirection = Quaternion.LookRotation(dirToEnemy, Vector3.up);
                GameObject hitEffect = Instantiate(hitEffectPrefabs[script.GetChargeLevel()], contactPoint, hitDirection);
                Destroy(hitEffect, 3.0f);
            }
        }

    }

    public void applyBuriedPhysics(GameObject enemyObj)
    {
        if (buriedWallNotify != null)
        {
            buriedWallNotify.GetWallScript().reaction(enemyObj, true);
            buriedWallNotify.ClearBuriedPlayer();
            buriedWallNotify = null;
        }
        selfCollider.isTrigger = false;
        isOver = true;
        isMoveFree = false;
        state = eState.State_Down;
    }

    protected void ReviveFromBuried()
    {
        if (buriedWallNotify != null)
        {
            buriedWallNotify.ClearBuriedPlayer();
            buriedWallNotify = null;
        }

        selfCollider.isTrigger = false;
        isOver = false;
        isMoveFree = false;
        downTimer = 0.0f;
        downEffectTimer = 0.0f;
        downObjRenderer.enabled = false;
        boostDir = Vector3.zero;
        state = eState.State_NoInput;

        Vector3 reverseWallVec = -wallVec;
        transform.rotation = Quaternion.LookRotation(reverseWallVec, Vector3.up);
        transform.position += reverseWallVec;
        v = reverseWallVec.normalized;
        wallVec = Vector3.zero;
        isCollide = false;

        //無敵時間の設定
        isInvincible = true;
        invincibleTimer = 1.5f;
        velocityResetTimer = 0.25f;

        buriedReviveValue = 0.0f;

        hitStop = false;
        hitStopTimer = 0.0f;
        postHitStopVelocity = Vector3.zero;
    }

    protected void OnCollisionExit(Collision collision)
    {
        //離れたときにisCollideがfalseに戻る
        if (collision.gameObject.tag == "Player" && isCollide)
        {
            isCollide = false;
        }
        else if (collision.gameObject.tag == "CPU" && isCollide)
        {
            isCollide = false;
        }

        if (collision.gameObject.tag == "Wall" && isCollide)
        {
            isCollide = false;
        }

    }

    protected void OnTriggerEnter(Collider other)
    {
        if (other.CompareTag("Death"))
        {
            return;
        }
        else if (other.CompareTag("BombExp"))
        {
            downTimer = 0.0f;
            downEffectTimer = 0.0f;
            if (!isDownObjDisabled)
            {
                downObjRenderer.enabled = true;
            }

            downBoostFactor += downBoostAddValue * 2;
            arwMatObj.SetActive(false);
            isMoveFree = false;
            isCollide = true;
            Debug.Log("c");
            Vector3 vec = transform.position - other.transform.position;
            vec = vec.normalized * 2 * downBoostFactor;
            vec.y = 0;
            v = vec;
            Pauser.Pause();
            state = eState.State_Down;
        }
    }

    //死んだときにデスゾーンから呼ばれる
    public void Death()
    {
        if (buriedWallNotify != null)
        {
            buriedWallNotify.ClearBuriedPlayer();
            buriedWallNotify = null;
        }
        selfCollider.isTrigger = false;
        isAnimDeathFlag = true;
        lifeState = eLifeState.LifeState_Death;
        //Debug.Log("Death");
        SingleSound deathSoundObj = new GameObject("DeathSound").AddComponent<SingleSound>();
        deathSoundObj.SetupSound(transform.position, deathSound2, 1.0f);
        Vector3 deathCrackPos = new Vector3(transform.position.x, deathCrackY, transform.position.z);
        float randomRotY = Random.Range(0f, 360f);
        InstantiateAsync(deathCrack, deathCrackPos, Quaternion.Euler(0, randomRotY, 0));
        score = Mathf.Max(score - deathPenalty, 0);

        //最後にあたったプレイヤーがいたなら（downせず自爆しても適用される）
        if (lastHitPlayer != null)
        {
            //そのプレイヤーのスコアを加算する
            lastHitPlayer.AddScore(1 + lastHitPlayer.GetBonusScore());
            lastHitPlayer = null;
        }
        // this.gameObject.SetActive(false);
        /*
        foreach (Renderer renderer in renderers)
        {
            renderer.enabled = false;
        }
         */

        SetVelocitySafe(rb, Vector3.zero);
        rb.isKinematic = true;

        downMatObj.SetActive(false);
        arwMatObj.SetActive(false);

        hitStop = false;
        hitStopTimer = 0.0f;
        currentHitStopDuration = 0.0f;
        postHitStopVelocity = Vector3.zero;

        this.gameObject.GetComponent<SphereCollider>().enabled = false;
        anim.SetInteger("lifeState", (int)lifeState);
        transform.LookAt(Camera.main.transform.position);

        StopAllPadMotor();

        audioSource.Stop();

        if (currentChargeEffect != null)
        {
            Destroy(currentChargeEffect);
            currentChargeEffect = null;
            lastChargeLevel = -1;
            chargeTimer = 0.0f;
        }

        DeactivateFootUI(1f);

        this.enabled = false;
    }

    [Serializable]
    public struct MotorData
    {
        [Header("振動の強さ (0.0 ~ 1.0)")]
        public float amplitude;
        [Header("振動の持続時間 (秒)")]
        public float duration;

        public MotorData(float amplitude, float duration)
        {
            this.amplitude = amplitude;
            this.duration = duration;
        }
    }

    private Coroutine currentMotorCoroutine = null;
    private float currentMotorAmplitude = 0f;
    public void RegisterPadMotor(MotorData motorData)
    {
        float amplitude = motorData.amplitude;
        float duration = motorData.duration;
        if (controllerDevice != null)
        {
            if (!(controllerDevice is Gamepad gamepad))
            {
                return;
            }
            if (amplitude < currentMotorAmplitude)
            {
                // より強い振動が既に登録されている場合は無視
                return;
            }
            if (currentMotorCoroutine != null)
            {
                StopCoroutine(currentMotorCoroutine);
            }
            if (Mathf.Abs(amplitude - currentMotorAmplitude) > Mathf.Epsilon)
            {
                currentMotorAmplitude = amplitude;
                gamepad.SetMotorSpeeds(amplitude, amplitude);
            }
            currentMotorCoroutine = StartCoroutine(StopPadMotorAfterDuration(duration));
        }
    }

    private IEnumerator StopPadMotorAfterDuration(float duration)
    {
        float elapsed = 0f;
        while (elapsed < duration)
        {
            elapsed += Time.deltaTime;
            yield return null;
        }
        if (controllerDevice is Gamepad gamepad)
        {
            gamepad.SetMotorSpeeds(0, 0);
        }
        currentMotorCoroutine = null;
        currentMotorAmplitude = 0f;
    }

    public void StopAllPadMotor()
    {
        if (controllerDevice is Gamepad gamepad)
        {
            gamepad.SetMotorSpeeds(0, 0);
            if (currentMotorCoroutine != null)
            {
                StopCoroutine(currentMotorCoroutine);
                currentMotorCoroutine = null;
                currentMotorAmplitude = 0f;
            }
        }
    }

    public eState GetState()
    {
        return state;
    }

    public void SetState(eState playerState)
    {
        state = playerState;
        if (playerState == eState.State_Idle || playerState == eState.State_None)
            isMoveFree = true;

        if (playerState != eState.State_Buried && selfCollider.isTrigger)
            selfCollider.isTrigger = false;
    }

    public eLifeState GetLifeState()
    {
        return lifeState;
    }

    public void SetLifeState(eLifeState playerlifeState)
    {
        lifeState = playerlifeState;
    }

    public float GetVelocityMagnitude()
    {
        return v.magnitude;
    }

    public Vector3 GetVelocity()
    {
        return v;
    }

    public void SetVelocity(Vector3 velocity)
    {
        v = velocity;
    }

    public void SetMoveFree(bool isMove)
    {
        isMoveFree = isMove;
    }

    public int GetScore()
    {
        return score;
    }

    public void AddScore(int value)
    {
        score += value;
        GameObject scoredPointObj = Instantiate(scoredPointPrefab);
        Vector3 offset = new Vector3(0, 0.75f, 0);
        scoredPointObj.GetComponent<ScoredPoint>().ShowPoint(transform.position + offset, value);
    }

    public void SetLastHitPlayer(PlayerEntity player)
    {
        lastHitPlayer = player;
    }

    public Material GetNeonMaterial()
    {
        return neonMat;
    }

    public WallNotify GetBuriedWallNotify()
    {
        return buriedWallNotify;
    }

    public float GetBuriedReviveValue()
    {
        return buriedReviveValue;
    }

    public float GetCurrentPushPower()
    {
        return pushPowers[chargeLevel];
    }

    public int GetChargeLevel()
    {
        return chargeLevel;
    }

    public void SetHitStop(Vector3 postVelocity, float duration)
    {
        if (Mathf.Sign(v.x) != Mathf.Sign(postVelocity.x))
        {
            baseBoostX *= -1;
        }
        if (Mathf.Sign(v.z) != Mathf.Sign(postVelocity.z))
        {
            baseBoostZ *= -1;
        }
        if (duration > 0.0f)
        {
            hitStop = true;
            hitStopTimer = 0.0f;
            currentHitStopDuration = duration;
            postHitStopVelocity = postVelocity;
            SetVelocitySafe(rb, Vector3.zero);
            rb.isKinematic = true;
            v = Vector3.zero;
        }
        else
        {
            v = postVelocity;
            currentHitStopDuration = 0.0f;
            hitStopTimer = 0.0f;
        }
    }

    public bool IsHitStop()
    {
        return hitStop;
    }

    private Coroutine footUIAnimationCoroutine = null;
    protected void DeactivateFootUI(float seconds)
    {
        if (footUIAnimationCoroutine != null)
        {
            StopCoroutine(footUIAnimationCoroutine);
        }
        footUIAnimationCoroutine = StartCoroutine(FootUIAnimation(seconds, initialFootCircleUIScale, 0));
    }

    protected void ActivateFootUI(float seconds)
    {
        if (footUIAnimationCoroutine != null)
        {
            StopCoroutine(footUIAnimationCoroutine);
        }
        footUIAnimationCoroutine = StartCoroutine(FootUIAnimation(seconds, 0, initialFootCircleUIScale));
    }

    private IEnumerator FootUIAnimation(float seconds, float startScale, float endScale)
    {
        if (seconds <= 0f)
        {
            footCircleUI.transform.localScale = new Vector3(endScale, endScale, 1);
            yield break;
        }
        float timer = 0f;
        float currentScale = footCircleUI.transform.localScale.x;
        float totalDelta = endScale - startScale;
        if (Mathf.Approximately(totalDelta, 0f))
        {
            footCircleUI.transform.localScale = new Vector3(endScale, endScale, 1);
            yield break;
        }
        float progressed = (currentScale - startScale) / totalDelta;
        progressed = Mathf.Clamp01(progressed);
        float remainTime = seconds * (1f - progressed);

        while (timer < remainTime)
        {
            timer += Time.deltaTime;
            float t = (remainTime <= 0f) ? 1f : Mathf.Clamp01(timer / remainTime);
            float scale = Mathf.Lerp(currentScale, endScale, t);
            footCircleUI.transform.localScale = new Vector3(scale, scale, 1);
            yield return null;
        }
        footCircleUI.transform.localScale = new Vector3(endScale, endScale, 1);
    }

    protected void OnDisable()
    {
        StopAllPadMotor();
        if (audioSource != null)
            audioSource.Stop();
        if (footCircleUI != null)
            footCircleUI.SetActive(false);
    }

    protected void OnEnable()
    {
        if (footCircleUI != null)
            footCircleUI.SetActive(true);
    }

    protected void OnDestroy()
    {
        StopAllPadMotor();
    }

    protected void OnApplicationQuit()
    {
        if (controllerDevice is Gamepad gamepad)
        {
            gamepad.SetMotorSpeeds(0, 0);
            if (currentMotorCoroutine != null)
            {
                StopCoroutine(currentMotorCoroutine);
            }
        }
    }

    public void SetPadAssigner(PadAssigner assigner)
    {
        padAssigner = assigner;
    }

    protected void SetVelocitySafe(Rigidbody targetRb, Vector3 velocity)
    {
        if (targetRb != null && !targetRb.isKinematic)
        {
            targetRb.linearVelocity = velocity;
        }
    }

    public void IncreaseBonusScore(int value)
    {
        bonusAddScore += value;
    }

    public void DecreaseBonusScore(int value)
    {
        bonusAddScore = Mathf.Max(bonusAddScore - value, 0);
    }

    public int GetBonusScore()
    {
        return bonusAddScore;
    }

    public void IncreaseBonusWallScore(int value)
    {
        bonusWallAddScore += value;
    }

    public void DecreaseBonusWallScore(int value)
    {
        bonusWallAddScore = Mathf.Max(bonusWallAddScore - value, 0);
    }

    public int GetBonusWallScore()
    {
        return bonusWallAddScore;
    }

    public bool IsGamepadConnected()
    {
        return controllerDevice is Gamepad;
    }

    public bool IsJoystickConnected()
    {
        return controllerDevice is Joystick;
    }
}
