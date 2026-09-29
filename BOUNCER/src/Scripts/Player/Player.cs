
using UnityEngine;
using UnityEngine.InputSystem;

public class Player : PlayerEntity
{


    KeyCode chargeKey = KeyCode.Space;
    private PlayerInput playerInput;

    private Vector2 lastMoveInput = Vector2.zero;
    private bool inputCharging = false;
    private bool chargeReleased = false;
    
    [SerializeField, Header("回転速度")] private float rotateSpeed = 14.0f;
    
    // Start is called before the first frame update
    public override void Start()
    {
        base.Start();
        playerInput = gameObject.GetComponent<PlayerInput>();
        playerInput.actions["Charge"].started += (context) =>
        {
            inputCharging = true;
            if (state == eState.State_Buried)
            {
                buriedReviveValue += buriedReviveSpeed;
            }
        };
        playerInput.actions["Charge"].canceled += (context) =>
        {
            chargeReleased = true;
            inputCharging = false;
        };

    }

    // Update is called once per frame
    public override void Update()
    {
        if (!GameManager.instance.CanPlayerMove()) return;
        
        // baseのUpdate内でHitStopの時間経過処理をしているため、先にbaseを呼ばないとここで固まるので注意
        base.Update();
        
        if (hitStop) return;
        
        Vector2 moveInput = playerInput.actions["Move"].ReadValue<Vector2>();
        if (isMoveFree)
        {
            Quaternion hRotation = Quaternion.AngleAxis(Camera.main.transform.eulerAngles.y, Vector3.up);

            if (!IsJoystickConnected()) {
                v.x = moveInput.x;
                v.z = moveInput.y;
                // Debug.Log("Move Input: " + v);

                v = hRotation * v.normalized * moveSpeed;
            }

            if (state == eState.State_Charge)
            {
                if (!IsJoystickConnected()) {
                    arwMatObj.SetActive(true);
                }
                chargeTimer += Time.deltaTime;
                UpdateDirEffect();
                // if(Input.GetKeyUp(chargeKey))
                chargeLevel = Mathf.Min((int)(chargeTimer / chargeLevelInterval), 2);
                if (chargeReleased)
                {
                    if (!IsJoystickConnected()) {
                        if (chargeTimer > chargeMaxTime)
                        {
                            chargeTimer = chargeMaxTime;
                            chargeLevel = 2;
                        }
                        boostDir = transform.forward.normalized;
                        baseBoostX = boostDir.x * (boostDistances[chargeLevel] / boostTimes[chargeLevel]);
                        baseBoostZ = boostDir.z * (boostDistances[chargeLevel] / boostTimes[chargeLevel]);
                        v.x = baseBoostX;
                        v.z = baseBoostZ;
                        chargeTimer = 0;
                        boostTimer = 0;
                        isMoveFree = false;
                        arwMatObj.SetActive(false);
                        downObjRenderer.enabled = true;
                        downTimer = 0.0f;
                        state = eState.State_Boost;
                    } else
                    {
                        // 専用joystick接続時
                        // 状態リセットをするのみ
                        chargeTimer = 0;
                        chargeLevel = 0;
                        arwMatObj.SetActive(false);
                        state = eState.State_Idle;
                    }
                }
            }

            if (!IsJoystickConnected()) {
                Vector3 diff;
                diff = v;
                diff.y = 0;

                if (diff.magnitude > 0)
                {

                    Quaternion rotate = Quaternion.LookRotation(diff);

                    transform.rotation = Quaternion.RotateTowards(transform.rotation, rotate, rotateSpeed);
                }
            } else
            {
                if (moveInput.magnitude > 0.1f && isJoystickNeutral)
                {
                    isJoystickNeutral = false;
                    Vector3 viewVector = new Vector3(moveInput.x, 0, moveInput.y);
                    Quaternion rotate = Quaternion.LookRotation(hRotation * viewVector.normalized);
                    transform.rotation = rotate;

                    if (state == eState.State_Idle)
                    {
                        chargeLevel = 0;
                    }
                    boostDir = transform.forward.normalized;
                    baseBoostX = boostDir.x * (boostDistances[chargeLevel] / boostTimes[chargeLevel]);
                    baseBoostZ = boostDir.z * (boostDistances[chargeLevel] / boostTimes[chargeLevel]);
                    v.x = baseBoostX;
                    v.z = baseBoostZ;
                    chargeTimer = 0;
                    boostTimer = 0;
                    isMoveFree = false;
                    arwMatObj.SetActive(false);
                    downObjRenderer.enabled = true;
                    downTimer = 0.0f;
                    state = eState.State_Boost;
                }
            }
        }
        if (moveInput.magnitude < 0.1f && !isJoystickNeutral)
        {
            isJoystickNeutral = true;
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
            if (buriedReviveValue >= 1.0f)
            {
                ReviveFromBuried();
            }
        }


        // if (Input.GetKey(chargeKey) && state == eState.State_Idle)
        if (inputCharging && state == eState.State_Idle)
        {
            state = eState.State_Charge;
        }

        chargeReleased = false;
    }

    public override void LateUpdate()
    {
        base.LateUpdate();
    }
}
