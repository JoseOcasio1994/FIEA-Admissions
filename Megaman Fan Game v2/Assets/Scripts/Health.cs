using UnityEngine;
using System.Collections;

public class Health : MonoBehaviour {

    public int health;
    private int saveHealth;
    
    void Start()
    {
        //Saves health for re-activating from object pool
        saveHealth = health;
    }
	// Update is called once per frame
	void Update ()
    {
        if (health <= 0)
        {
            health = saveHealth;
            GameObjectUtil.Destroy(this.gameObject);
        }
    }

    void OnCollisionEnter2D(Collision2D coll)
    {

        if (coll.gameObject.tag == "Bullet")
            health -= coll.gameObject.GetComponent<Bullet>().Damage;

    }
}
