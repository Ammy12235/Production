using NUnit.Framework;
using System.Collections.Generic;
using UnityEngine;
[System.Serializable]
public enum InOutEventType
{
    InOrder,//リストに設定した順番にイベントを発火させる。前のイベントが終了したらすぐさま次のイベントに移る。
    InOrderSetDelay,//リストに設定した順番にイベントを発火させるが、イベントごとに遅延時間を設定できる
    AllAtOnce,//イベントを一気に発火させる。
    AllAtOnceSetDelay//イベントを一気に発火させるが、イベントごとに遅延時間を設定できる
}
[System.Serializable]
public struct EventInfo
{
    public float delay;
    public IEvent eventObject;
}

public class EventEmitter : MonoBehaviour
{

    [Header("イベントの発火方法(順番、一斉発火およびディレイを入れるかどうか)")]
    [SerializeField] private InOutEventType eventType;
    [Header("スタート時にイベントを発火させるかどうか")]
    [SerializeField] private bool isFireOnStart = false;//スタート時にイベントを発火させるかどうか
    [Header("一度きりのイベントかどうか")]
    [SerializeField] private bool isFireOnce = true;//一度だけ発動するか
    [Header("イベントの内容を設定するリスト")]
    [SerializeField] private List<EventInfo> eventInfoList;

    IEvent ownerEventObj;

    bool canFire = true;//イベントを発火させることができるかどうか

    public bool GetIsFireOnce()
    {
        return isFireOnce;
    }

    void Start()
    {
        if (isFireOnStart)
        {
            Fire();
        }
    }

    public void Fire()
    {
        if (canFire)
        {
            CreateEventActuator();
            if (isFireOnce)
            {
                canFire = false;
            }
        }
    }

    public bool GetCanFire()
    {
        return canFire;
    }

    private void CreateEventActuator()
    {
        GameObject obj = new GameObject("EventActuator");
        obj.transform.position = transform.position;
        EventActuator actuator = obj.AddComponent<EventActuator>();
        actuator.Delegation(this,eventInfoList, eventType);//イベント内容を丸ごと移譲させる

    }

    public void NotifyEventEnd()
    {
        if(ownerEventObj!=null)
        {
            ownerEventObj.Finish();
        }
    }

    public void SetOwnerEvent(IEvent eventObj)
    {
        ownerEventObj = eventObj;
    }
}
