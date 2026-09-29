using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using Unity.Cinemachine;

public class Wall : MonoBehaviour
{
    [SerializeField] int maxHP;
    [SerializeField] private float shakeTime;
    [SerializeField] private float shakeInterval;
    [SerializeField] private float shakeAmplitude;
    [SerializeField] private List<Material> materials;
    [SerializeField] private GameObject crackObj;
    [SerializeField] private GameObject crackSpecialObj;
    [SerializeField] private GameObject breakSoundObj;
    [SerializeField] private GameObject breakSpecialSoundObj;
    [SerializeField] private AudioClip wallHitSound1;
    [SerializeField] private AudioClip wallHitSound2;
    [SerializeField] private GameObject specialWallBreakableEffect;
    [SerializeField, Header("スペシャル壁の確率"), Range(0, 1)] private float specialWallProbability = 0.1f;
    [SerializeField, Header("スペシャル壁のマテリアル")] private Material specialMaterial;
    [SerializeField, Header("スペシャル壁の壊れそうな時のマテリアル")] private Material specialMaterial_damaged;
    [SerializeField, Header("スペシャル壁のスコア")] private int specialWallScore = 3;
    [SerializeField, Header("スペシャル壁のHP")] private int specialWallHP = 5;
    CinemachineImpulseSource impulseSource;
    bool isHit = false;
    bool isAnimated = false;
    bool isDestroy = false;
    int hp = 0;

    int defaultMaxHP;

    Vector3 initTrans;
    float shakeTimer = 0.0f;
    float shaleFactor = 0.0f;
    protected AudioSource audioSource;
    GameObject holdSpecalEffect;
    bool isSpecial = false;
    // Start is called before the first frame update
    void Start()
    {
        GetComponent<MeshRenderer>().material = materials[0];
        impulseSource = GetComponent<CinemachineImpulseSource>();
        audioSource = GetComponent<AudioSource>();
        initTrans = this.gameObject.transform.localScale;
        hp = maxHP;
        defaultMaxHP = maxHP;
        isSpecial = false;
    }

    // Update is called once per frame
    void Update()
    {
        if (isHit)
        {
            shakeTimer += Time.deltaTime;
            shaleFactor += shakeInterval;
            float value = shakeAmplitude * (Mathf.Sin(shaleFactor) / 2 + 0.5f) * (1 - shakeTimer / shakeTime);
            transform.localScale = initTrans + new Vector3(value, value, value);
            if (shakeTimer > shakeTime)
            {
                transform.localScale = initTrans;
                shakeTimer = 0.0f;
                isHit = false;
            }
        }


    }

    public void destroyWall()
    {
        isDestroy = true;
        if (isSpecial==false)
        {
            Instantiate(crackObj, transform.position, transform.rotation * Quaternion.Euler(0, 90, 90));
            Instantiate(breakSoundObj, transform.position, Quaternion.identity);
        }
        else
        {
            Instantiate(crackSpecialObj, transform.position, transform.rotation * Quaternion.Euler(0, 90, 90));
            Instantiate(breakSpecialSoundObj, transform.position, Quaternion.identity);
            if (holdSpecalEffect != null)
                Destroy(holdSpecalEffect);
        }

        impulseSource.GenerateImpulse();
        gameObject.SetActive(false);
    }

    public void regenerateWall()
    {
        if (holdSpecalEffect != null)
            Destroy(holdSpecalEffect);
        isDestroy = false;
        float rnd = Random.Range(0.0f, 1.0f);
        if (rnd < specialWallProbability)
        {
            isSpecial = true;
        }
        else
        {
            isSpecial = false;
        }

        if (!isSpecial)
        {
            GetComponent<MeshRenderer>().material = materials[0];
            maxHP = defaultMaxHP;
        }
        else
        {
            GetComponent<MeshRenderer>().material = specialMaterial;
            maxHP = specialWallHP;
        }
        hp = maxHP;
        gameObject.SetActive(true);
    }

    public bool reaction(Collision collision)
    {
        return reaction(collision.gameObject);
    }

    public bool reaction(GameObject obj, bool skipCheck = false)
    {
        if (hp <= 0)
        {
            return false;
        }
        if (obj.tag == "Player" || obj.tag == "CPU")
        {
            PlayerEntity script = obj.GetComponent<PlayerEntity>();
            if (script.GetState() == PlayerEntity.eState.State_Boost || script.GetState() == PlayerEntity.eState.State_Down || skipCheck)
            {
                isHit = true;
                shakeTimer = 0.0f;
                hp--;
                if (hp > 0)
                {
                    int hit = Random.Range(0, 2);
                    if (hit % 2 == 0) audioSource.PlayOneShot(wallHitSound1);
                    else audioSource.PlayOneShot(wallHitSound2);
                    if (!isSpecial)
                    {
                        GetComponent<MeshRenderer>().material = materials[maxHP - hp];
                    }
                    else
                    {
                        if (hp == 1)
                        {
                            GetComponent<MeshRenderer>().material = specialMaterial_damaged;
                            //holdSpecalEffect=Instantiate(specialWallBreakableEffect, transform.position, transform.rotation*Quaternion.Euler(-90,0,-180));
                        }
                    }

                }
                return true;


            }
        }

        return false;
    }

    public bool reactionTrigger(Collider collider)
    {

        if (collider.CompareTag("BombExp"))
        {
            isHit = true;
            shakeTimer = 0.0f;
            hp -= 2;
            if (hp > 0)
            {
                int hit = Random.Range(0, 2);
                if (hit % 2 == 0) audioSource.PlayOneShot(wallHitSound1);
                else audioSource.PlayOneShot(wallHitSound2);
                GetComponent<MeshRenderer>().material = materials[maxHP - hp];

            }
            return true;
        }
        return false;
    }

    public void ReplaceToSpecialWall()
    {
        isSpecial = true;
        GetComponent<MeshRenderer>().material = specialMaterial;
        maxHP = specialWallHP;
        hp = maxHP;
        isHit = true;
        shakeTimer = 0.0f;
    }

    public void ReplaceToNormalWall()
    {
        isSpecial = false;
        GetComponent<MeshRenderer>().material = materials[0];
        maxHP = defaultMaxHP;
        hp = maxHP;
        isHit = true;
        shakeTimer = 0.0f;
    }

     private GameObject InstantiateAsync(GameObject prefab, Vector3 position, Quaternion rotation)
    {
        GameObject obj = null;
        obj = GameObject.Instantiate(prefab, position, rotation);
        return obj;
    }

    public bool GetIsDestroy()
    {
        return isDestroy;
    }

    public int GetHP()
    {
        return hp;
    }

    public bool GetIsSpecial()
    {
        return isSpecial;
    }

    public int GetSpecialWallScore()
    {
        return specialWallScore;
    }

}
