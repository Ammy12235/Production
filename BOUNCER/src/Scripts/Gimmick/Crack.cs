using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Crack : MonoBehaviour
{
    Rigidbody rb;
    private float time;
    [SerializeField] float power;
    const float disappearBiginTime = 0.8f;//小さくなり始める時間
    const float deleteTime = 1;//消える時間

    // Start is called before the first frame update
    void Start()
    {
        rb = GetComponent<Rigidbody>();
        rb.AddForce(transform.forward * 3, ForceMode.Impulse);
        rb.linearVelocity += new Vector3(0, power, 0);
    }

    // Update is called once per frame
    void Update()
    {
        time += Time.deltaTime;

        if (time > disappearBiginTime)
        {
            if (transform.localScale.x > 0)
            {
                transform.localScale -= new Vector3(0.02f, 0.02f, 0.02f);
            }
        }

        if (time > deleteTime)
        {
            Destroy(this.gameObject);
        }
    }

    private void FixedUpdate()
    {
        rb.linearVelocity += new Vector3(0, -0.5f, 0);//下方向へ加速
    }

}
