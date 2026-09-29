
using UnityEngine;
using Unity.Cinemachine;

public class DeathZone : MonoBehaviour
{
    [SerializeField] CinemachineImpulseSource impulseSource;
    // Start is called before the first frame update
    void Start()
    {
        if (impulseSource == null)
        {
            impulseSource = GetComponent<CinemachineImpulseSource>();
        }
    }

    // Update is called once per frame
    void Update()
    {
        
    }

    private void OnTriggerEnter(Collider other)
    {
        //プレイヤーがデスゾーンに触れたら
        if(other.CompareTag("Player")||other.CompareTag("CPU"))
        {
            //
            if (other.gameObject.GetComponent<PlayerEntity>().enabled == false) return;
            //ゲームマネージャーに死んだことを通達し、落ちてきたプレイヤーには死んだことを宣言する
            GameManager.instance.NotifyIsDead(other.gameObject, true);
            other.gameObject.GetComponent<PlayerEntity>().Death();
            
            impulseSource.GenerateImpulse();
        }
    }
}
