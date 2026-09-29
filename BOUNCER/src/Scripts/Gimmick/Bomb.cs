
using UnityEngine;
using Unity.Cinemachine;
using static PlayerEntity;

public class Bomb : MonoBehaviour
{
    [SerializeField] private float expTime;
    [SerializeField] private GameObject expObj;
    [SerializeField] private GameObject rimLightObj;
    [SerializeField] private float rimPower;
    [SerializeField] private float expFlushFirstTime;
    [SerializeField] private float expFlushFirstFreq;
    [SerializeField] private float expFlushSecondTime;
    [SerializeField] private float expFlushSecondFreq;
    [SerializeField] private float expFlushThirdTime;
    [SerializeField] private float expFlushThirdFreq;

    AudioSource audioSource;
    [SerializeField] AudioClip assertSound;
    RimLight rimLightShader;
    CinemachineImpulseSource impusleSource;
    float timer;
    float soundTimer = 0;
    // Start is called before the first frame update
    void Start()
    {
        rimLightShader=rimLightObj.GetComponent<RimLight>();
        impusleSource = GetComponent<CinemachineImpulseSource>();
        audioSource = GetComponent<AudioSource>();
    }

    // Update is called once per frame
    void Update()
    {
        timer += Time.deltaTime;
        if(timer>expTime)
        {
            Debug.Log("Explosion");
            Instantiate(expObj,transform.position,Quaternion.identity);
            impusleSource.GenerateImpulse();
            Destroy(this.gameObject);
        }

        if(timer>0&&timer< expFlushFirstTime)
        {
            float value = (Mathf.Sin(expFlushFirstFreq*2*Mathf.PI*timer)+1)/2*rimPower;
            rimLightShader.setRimPower(value);

            soundTimer += Time.deltaTime;
            if (soundTimer > 1/expFlushFirstFreq)
            {
                audioSource.PlayOneShot(assertSound);
                soundTimer = 0.0f;
            }
        }
        else if (timer > expFlushFirstTime && timer < expFlushSecondTime)
        {
            float value = (Mathf.Sin(expFlushSecondFreq * 2 * Mathf.PI * timer) + 1) / 2 * rimPower;
            rimLightShader.setRimPower(value);
            soundTimer += Time.deltaTime;
            if (soundTimer > 1 / expFlushSecondFreq)
            {
                audioSource.PlayOneShot(assertSound);
                soundTimer = 0.0f;
            }
        }
        else if (timer > expFlushSecondTime && timer < expFlushThirdTime)
        {
            float value = (Mathf.Sin(expFlushThirdFreq * 2 * Mathf.PI * timer) + 1) / 2 * rimPower;
            rimLightShader.setRimPower(value);
            soundTimer += Time.deltaTime;
            if (soundTimer > 1 / expFlushThirdFreq)
            {
                audioSource.PlayOneShot(assertSound);
                soundTimer = 0.0f;
            }
        }
    }
    private void OnTriggerEnter(Collider other)
    {
        if (other.CompareTag("Death"))
        {

            Instantiate(expObj, transform.position, Quaternion.identity);
            impusleSource.GenerateImpulse();
            Destroy(this.gameObject);
            return;
        }
    }
}
