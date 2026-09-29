
using System.Collections;
using UnityEngine;

public class InGameCamera : MonoBehaviour
{

    public Transform Target;
    [SerializeField] private float speed=3.0f;

    private Hashtable _moveArgs;
    private Hashtable _rotateArgs;

    void Awake() {
        _moveArgs = iTween.Hash(
            "position", Vector3.zero,
            "time", speed
        );
        _rotateArgs = iTween.Hash(
            "rotation", Vector3.zero,
            "time", speed
        );

        iTween.Init(gameObject);
    }
    
    void FixedUpdate() {
        if (GameManager.instance.GetGameState()==GameManager.eGameState.GameState_RoundStart||
            GameManager.instance.GetGameState() == GameManager.eGameState.GameState_Rounding||
            GameManager.instance.GetGameState() == GameManager.eGameState.GameState_SuddenDeathStart||
            GameManager.instance.GetGameState() == GameManager.eGameState.GameState_SuddenDeathRounding||
            GameManager.instance.GetGameState() == GameManager.eGameState.GameState_Finish)
        {
            _moveArgs["position"] = Target.position;
            _moveArgs["time"] = speed;
            
            _rotateArgs["rotation"] = Target.rotation.eulerAngles;
            _rotateArgs["time"] = speed;
            
            iTween.MoveUpdate(this.gameObject, _moveArgs);
            iTween.RotateUpdate(this.gameObject, _rotateArgs);
        }
    }
}