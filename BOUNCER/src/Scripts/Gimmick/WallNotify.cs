using UnityEngine;

public class WallNotify : MonoBehaviour
{
    [SerializeField] private Wall wall;
    bool isCollide=false;

    [SerializeField] private float regenerateTime = 5.0f;
    private float regenerateTimer = 0.0f;
    [SerializeField] private Collider wallCollider;

    private GameObject buriedPlayer;
    // Start is called before the first frame update
    void Start()
    {
        //wall = GetComponentInChildren<Wall>();
    }

    // Update is called once per frame
    void Update()
    {
        if (wall.GetHP() <= 0 && !wall.GetIsDestroy())
        {
            wall.destroyWall();
            wallCollider.enabled = false;
        }

        if (wall.GetIsDestroy())
        {
            regenerateTimer += Time.deltaTime;
            if (regenerateTimer >= regenerateTime && GameManager.instance.GetGameState() != GameManager.eGameState.GameState_SuddenDeathRounding)
            {
                wall.regenerateWall();
                ClearBuriedPlayer();
                wallCollider.enabled = true;
                regenerateTimer = 0.0f;
            }
        }

        if (GetBuriedPlayer() != null)
        {
            PlayerEntity playerEntity = GetBuriedPlayer().GetComponent<PlayerEntity>();
            if (playerEntity != null && playerEntity.GetState() != PlayerEntity.eState.State_Buried)
            {
                ClearBuriedPlayer();
            }
        }
    }

    public Wall GetWallScript()
    {
        return wall;
    }

    public GameObject GetBuriedPlayer()
    {
        if (buriedPlayer == null)
        {
            return null;
        }
        PlayerEntity playerEntity = buriedPlayer.GetComponent<PlayerEntity>();
        if (playerEntity == null || playerEntity.GetBuriedWallNotify() != this)
        {
            ClearBuriedPlayer();
            return null;
        }
        return buriedPlayer;
    }

    public void SetBuriedPlayer(GameObject player)
    {
        buriedPlayer = player;
    }

    public void ClearBuriedPlayer()
    {
        buriedPlayer = null;
    }

    public void ReplaceToSpecialWall(bool respawnDestroyedWalls = false)
    {
        if (wall.GetIsDestroy() && respawnDestroyedWalls)
        {
            wall.regenerateWall();
            wallCollider.enabled = true;
            regenerateTimer = 0.0f;
        }
        wall.ReplaceToSpecialWall();
    }

    public void ReplaceToNormalWall(bool respawnDestroyedWalls = false)
    {
        if (wall.GetIsDestroy() && respawnDestroyedWalls)
        {
            wall.regenerateWall();
            wallCollider.enabled = true;
            regenerateTimer = 0.0f;
        }
        wall.ReplaceToNormalWall();
    }

    private void OnCollisionStay(Collision collision)
    {
        // if(!isCollide)
        // {
        //     if(wall.reaction(collision))
        //     isCollide = true;
        // }
        
    }

    private void OnCollisionExit(Collision collision)
    {
        // isCollide = false;
    }

    private void OnTriggerEnter(Collider other)
    {
        wall.reactionTrigger(other);
            
    }


}
