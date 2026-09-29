using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class ReviveSlider : MonoBehaviour
{
    PlayerEntity playerEntity;
    // Start is called before the first frame update
    void Start()
    {
        playerEntity = GetComponentInParent<PlayerEntity>();
    }

    // Update is called once per frame
    void Update()
    {
        if (playerEntity.GetState() == PlayerEntity.eState.State_Buried)
        {
            float value = playerEntity.GetBuriedReviveValue();
            transform.localScale = new Vector3(value, 1.0f, 1.0f);

            Vector3 dir = Camera.main.transform.position - transform.position;
            Vector3 up = Camera.main.transform.up;
            transform.parent.transform.rotation = Quaternion.LookRotation(dir, up);
        }
        else
        {
            transform.localScale = new Vector3(0.0f, 1.0f, 1.0f);
        }
    }
}
