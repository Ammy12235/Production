
using System.Collections.Generic;
using UnityEngine;

public class GetCenterPoint : MonoBehaviour
{
    [SerializeField]List<Transform> transList = new();   //カメラ範囲内におさめたいオブジェクトのリスト
    [SerializeField] List<Transform> anchorList = new(); //プレイヤー以外の画面に収めるオブジェクトのリスト
    [SerializeField] private Camera usingCamera; 
    [SerializeField] private Transform cameraLookPos;
    
    private Vector3 pos = new Vector3();
    private Vector3 center = new Vector3();
    [SerializeField] private float maxDistance = 1.0f;
    [SerializeField] private float maxRadius = 1.0f;
    private float radius;
    [SerializeField] private float margin = 1.0f;        //半径を少し余分にとるための値
    private float distance;
    [SerializeField] private float cameraHeight = 1.5f;  //カメラが地面にめり込まないようにカメラを浮かせる高さ

    void Start()
    {
        //cameraPos = GameObject.Find("CameraPosition").GetComponent<Transform>();
        for (int i = 0; i < GameManager.instance.playerList.Count; i++)
        {
            transList[i] = GameManager.instance.playerList[i].transform;

        }
            }

    public void SetTrackPlayerList(List<GameObject> member)
    {
        for (int i = 0; i < GameManager.instance.playerList.Count; i++)
        {
            if (member[i]==null)
            {
                transList[i] = null;
            }
            else transList[i] = member[i].transform;

        }
        
    }

    void Update()
    {
        pos = new Vector3(0, 0, 0);
        radius = 0.0f;
        int count = 0;
        for (int i = 0; i < transList.Count; i++)
        {
            if (GameManager.instance.GetIsDead()[i] == false)
            {
                pos += transList[i].position;
                count++;
            }

        }
        for(int i=0;i<anchorList.Count;i++)
        {
            pos += anchorList[i].position;
            count++;
        }
        if (count > 0)//プレイヤーが生き残っていたら位置の平均点を見るべき点として指定する
        {
            center = pos / count;
            float distance = Vector3.Distance(center, new Vector3(0, 0, 0));
            if (distance > maxRadius)
            {
                center *= maxRadius / distance;

            }
        }
        else center = new Vector3(0, 0, 0);//プレイヤーが一人もいなかったら座標の真ん中を見るべき点として指定する
        this.transform.position = center;           //CenterPointのポジションを中心に配置

        //カメラに収める際に一番カメラを引く必要がある点を算出
        for (int i = 0; i < transList.Count; i++)
        {
            if (GameManager.instance.GetIsDead()[i] == false)
            {
                radius = Mathf.Max(radius, Vector3.Distance(center, transList[i].position));
            }
        }
        for (int i = 0; i < anchorList.Count; i++)
        {
            radius = Mathf.Max(radius, Vector3.Distance(center, anchorList[i].position));
        }
        distance = (radius + margin) / Mathf.Sin(usingCamera.fieldOfView * 0.5f * Mathf.Deg2Rad);   //カメラの距離を算出
        if (distance > maxDistance)
        {
            distance = maxDistance;
        }
        cameraLookPos.localPosition = new Vector3(0, cameraHeight, -distance);  //CameraPositionをカメラの距離をもとに配置
        cameraLookPos.LookAt(this.transform);           //CameraPositionを中心の方向に向かせる
    }
}