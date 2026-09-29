
using UnityEngine;

public class ExpObject : MonoBehaviour
{
    
    private float time;
    [SerializeField] float appearEndTime = 0.8f;//なり大きくなり終わる時間
    [SerializeField] float disappearBiginTime = 0.8f;//小さくなり始める時間
    [SerializeField] GameObject rimObj;
    [SerializeField] GameObject particleObj;
    [SerializeField] float deleteTime = 1;//消える時間
    float rimPower = 0;
    RimLight rimLightShader;
    ParticleSystem particle;
    SphereCollider collider;
    
    // Start is called before the first frame update
    void Start()
    {
        rimLightShader = rimObj.GetComponent<RimLight>();
        collider = GetComponent<SphereCollider>();
        particle = particleObj.GetComponent<ParticleSystem>();
        rimObj.transform.localScale = transform.localScale*3;
        
      

    }

    // Update is called once per frame
    void Update()
    {
        time += Time.deltaTime;
        var main = particle.main;  // MainModule の取得
        Color startColor = main.startColor.color;  // 現在のstartColorを取得
        startColor.a -= time;  // アルファ値を変更
        main.startColor = startColor;  // 更新された色をセット
        rimPower = (deleteTime - time*2)/deleteTime;
        if (rimPower < 0) rimPower = 0;
        rimLightShader.setRimPower(rimPower);
        if (time > appearEndTime&& time < disappearBiginTime)
        {
            if (transform.localScale.x > 0)
            {
               
                rimObj.transform.localScale+= new Vector3(Time.deltaTime, Time.deltaTime, Time.deltaTime);
               
               
            }
        }
        if(time > appearEndTime)
        {
            collider.enabled = false;
        }

        if(time > disappearBiginTime)
        {
            Pauser.Resume();
        }
        

        if (time > deleteTime)
        {
            
            Destroy(this.gameObject);
        }
    }
}
