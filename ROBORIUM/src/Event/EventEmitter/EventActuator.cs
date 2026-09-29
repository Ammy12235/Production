using System.Collections.Generic;
using UnityEngine;

/// <summary>
/// イベント発火元が消滅しても代わりにイベントを発火させるためのクラス
/// </summary>
public class EventActuator : MonoBehaviour
{
    [System.Serializable]
    public struct ActInfo
    {
        public float delay;
        public bool isFired;
    }
    [SerializeField] private List<EventInfo> _info = new();

    private InOutEventType eventType;

    EventEmitter motherEmitter;

    int eventCounter = 0;//イベントの実行回数を数えるための変数
    [SerializeField, ReadOnly] private List<ActInfo> actInfoList = new();

    //イベントを移譲するための関数。イベント発火元がイベントを発火させるときに呼び出す
    public void Delegation(EventEmitter eventEmitter, List<EventInfo> info, InOutEventType type)
    {
        motherEmitter = eventEmitter;
        eventType = type;
        _info = info;
        foreach (var it in _info)//イベントオブジェクトにこのクラスを所有者として設定する
        {
            it.eventObject.SetOwnerActuator(this);

            ActInfo actInfo;
            actInfo.isFired = false;
            actInfo.delay = it.delay;
            actInfoList.Add(actInfo);
        }

        eventCounter = 0;

        if (eventType == InOutEventType.InOrder)
        {
            InOutEventSystem.Instance.SetEvent(_info[eventCounter].eventObject);
        }
        else if (eventType == InOutEventType.AllAtOnce)
        {
            foreach (var it in _info)
            {
                InOutEventSystem.Instance.SetEvent(it.eventObject);
            }
        }

    }

    //更新内ではイベントの遅延時間を減らしていき、遅延時間が0以下になったイベントを発火させる
    void Update()
    {

        if (eventType == InOutEventType.InOrderSetDelay)
        {
            ActInfo actInfoTmp = actInfoList[eventCounter];
            actInfoTmp.delay -= Time.deltaTime;

            if (actInfoTmp.delay <= 0 && actInfoTmp.isFired == false)//もしイベントの遅延時間が0以下になったら、イベントを発火させる
            {
                actInfoTmp.isFired = true;
                InOutEventSystem.Instance.SetEvent(_info[eventCounter].eventObject);
            }

            actInfoList[eventCounter] = actInfoTmp;

        }
        else if (eventType == InOutEventType.AllAtOnceSetDelay)
        {
            for (int i = 0; i < _info.Count; i++)
            {
                ActInfo actInfoTmp = actInfoList[i];
                actInfoTmp.delay -= Time.deltaTime;

                if (actInfoTmp.delay <= 0 && actInfoTmp.isFired == false)//もしイベントの遅延時間が0以下になったら、イベントを発火させる
                {
                    actInfoTmp.isFired = true;
                    InOutEventSystem.Instance.SetEvent(_info[i].eventObject);
                }
                actInfoList[i] = actInfoTmp;
            }
        }
    }


    //イベントが終了したときに呼び出される関数。イベントオブジェクトから呼び出される
    public void NotifyEventFinished(IEvent eventObject)
    {

        //イベントが終了するたびにイベントの実行回数を数える。すべてのイベントが終了したら、このクラスを消滅させる
        eventCounter++;
        if (eventCounter >= _info.Count)
        {
            if (motherEmitter != null)
            {
                motherEmitter.NotifyEventEnd();
            }

            Destroy(gameObject);

        }

        //順番に実行するなら、次のイベントを発火させる
        if (eventType == InOutEventType.InOrder)
        {
            InOutEventSystem.Instance.SetEvent(_info[eventCounter].eventObject);
        }

    }

    private void OnDestroy()
    {

    }
}
