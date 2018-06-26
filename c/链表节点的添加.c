#include<stdio.h>
#include<stdlib.h>

struct node 
{
	int data;
	struct node *next;
};

int main()
{
	struct node *hand,*p,*q,*t,*k;
	int i,l,n,a,b,j;
	hand = NULL;
	printf("please input a number:");
	scanf("%d",&n);
	for(i = 0;i < n; i++)
	 {		
	   p = (struct node  *)malloc(sizeof(struct node ));
	   scanf("%d",&a);
	   p->data = a;
	   p->next = NULL;
	   if(hand ==NULL)
		   hand = p;
	   else
		   q->next = p;
	   
	   q = p;
	 }
	 
	 printf("please input a number to add:");
	 scanf("%d",&b);
	 k = hand;
	 k = (struct node  *)malloc(sizeof(struct node ));
	 k->data = b;
	 k->next = NULL;
	 
	 t =  hand;
	 while (t != NULL)
	 {
		 if( k->data>= t->data &&  k->data<t->next)
		 {
			 k->next = t->next;
			 t->next = k;
			 break;
		 }
		 else 
		 {
			 t = t->next;
		 }
	 }
	 t =  hand;
	 while (t != NULL)
	 {
		 printf("%d ",t->data);
		 t = t->next;
	 }
	printf("\n");
	
	return 0;
}
