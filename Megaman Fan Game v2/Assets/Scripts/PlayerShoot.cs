using UnityEngine;
using System.Collections;
using UnityStandardAssets.CrossPlatformInput;

public class PlayerShoot : MonoBehaviour
{

    private Rigidbody2D rbody;
    private Animator anim;

    //public GameObject bullets;
    public GameObject BulletPrefab;
    public float Damage = 1, BulletSpeed, fireRate;
    private float lastShot = 0f, gravity;

    // Use this for initialization
    void Start()
    {
        rbody = GetComponent<Rigidbody2D>(); 
        anim = GetComponent<Animator>();
    }

    void Update()
    {

        // Shoot
        if (Time.time > fireRate + lastShot)
        {
            // Normal Shoot
            if (CrossPlatformInputManager.GetButtonDown("Fire")) //Replaced "Input.GetKeyDown(KeyCode.J)" with button inputs
            {
                anim.SetBool("isAttack", true);
                Fire();
            }
            else
            {
                anim.SetBool("isAttack", false);
            }
        }

    }

    void Fire()
    {
        GameObject Clone;

        Clone = GameObjectUtil.Instantiate(BulletPrefab,
            new Vector2(rbody.transform.position.x + 1f, rbody.transform.position.y + .1f));
        Clone.GetComponent<Bullet>().SetStartX(rbody.transform.position.x);
        Clone.gameObject.SetActive(true);
        Clone.GetComponent<Rigidbody2D>().velocity = new Vector2(BulletSpeed, 0);
            

        //Below not needed only faces one way -JT
        /* else
         {
             Clone = GameObjectUtil.Instantiate(BulletPrefab,
                     new Vector2(transform.position.x - 1f, transform.position.y + .1f)); // JT as Rigidbody2D;
             Clone.gameObject.SetActive(true);

             Clone.GetComponent<Rigidbody2D>().velocity = new Vector2(BulletSpeed * -1, 0);
         }*/

        lastShot = Time.time;
    }

}